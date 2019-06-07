//
// Created by Max Vissing on 2019-06-07.
//

#include "HKClient.h"

HKClient::HKClient(HKServer *server, ClientContext* client) : WiFiClient(client), server(server), verifyContext(nullptr), encrypted(false), readKey(), countReads(0), writeKey(), countWrites(0), pairingId(0), permission(0) {}

HKClient::~HKClient() {
    delete verifyContext;
}

bool HKClient::readBytesWithTimeout(size_t maxLength, std::vector<byte> &data, int timeout_ms) {
    while (data.size() < maxLength) {
        int tries = timeout_ms;
        size_t avail;
        while (!(avail = available()) && tries--) {
            delay(1);
        }
        if (!avail) {
            break;
        }
        if (data.size() + avail > maxLength) {
            avail = maxLength - data.size();
        }
        while (avail--) {
            data.push_back(read());
        }
    }
    return data.size() == maxLength;
}

bool HKClient::received() {
    size_t dataSize;
    byte *data;
    size_t dataPos = 0;
    if (encrypted) {
        data = receivedDecrypted(dataSize);
    } else {
        dataSize = available();
        data = (byte *) malloc(dataSize);
        readBytes(data, dataSize);
    }

    String complete = String((char *) data);

    for (size_t i = dataPos; i < dataSize; i++) {
        if (data[i] == '\r') {
            data[i] = 0;
            break;
        }
    }
    String req = String((char *)data);
    dataPos += req.length() + 2;

    int addrStart = req.indexOf(' ');
    int addrEnd = req.indexOf(' ', addrStart + 1);
    if (addrStart == -1 || addrEnd == -1) {
        Serial.println("Could not parse address");
        free(data);
        return false;
    }

    String methodStr = req.substring(0, addrStart);
    String url = req.substring(addrStart + 1, addrEnd);
    String versionEnd = req.substring(addrEnd + 8);
    // _currentVersion = atoi(versionEnd.c_str());
    String searchStr = "";
    int hasSearch = url.indexOf('?');
    if (hasSearch != -1) {
        searchStr = url.substring(hasSearch + 1);
        url = url.substring(0, hasSearch);
    }

    HTTPMethod  method = HTTP_GET;
    if (methodStr == "POST") {
        method = HTTP_POST;
    } else if (methodStr == "PUT") {
        method = HTTP_PUT;
    }

    String formData;
    if (method == HTTP_POST || method == HTTP_PUT) {
        String headerName;
        String headerValue;
        String contentType;
        uint32_t contentLength = 0;

        while (true) {
            for (size_t i = dataPos; i < dataSize; i++) {
                if (data[i] == '\r') {
                    data[i] = 0;
                    break;
                }
            }
            req = String((char *)data + dataPos);
            dataPos += req.length() + 2;

            if (req == "") {
                break;
            }

            int headerDiv = req.indexOf(':');
            if (headerDiv == -1) {
                break;
            }
            headerName = req.substring(0, headerDiv);
            headerValue = req.substring(headerDiv + 1);
            headerValue.trim();

            if (headerName.equalsIgnoreCase("Content-Type")) {
                contentType = headerValue;
            } else if (headerName.equalsIgnoreCase("Content-Length")) {
                contentLength = headerValue.toInt();
            }
        }

        std::vector<byte> plainBuf;
        while (dataPos < dataSize) {
            plainBuf.push_back((byte) data[dataPos]);
            dataPos++;
        }

        if (!encrypted && plainBuf.size() < contentLength) {
            if (!readBytesWithTimeout(contentLength, plainBuf, 2000)) {
                Serial.println("Timeout: " + complete);
                return false;
            }
        }

        if (plainBuf.size() < contentLength) {
            Serial.println("Data too short: " + complete);

            free(data);
            return false;
        }


        Serial.println("received: Method=" + methodStr + " URL=" + url + " complete=");
        Serial.println(complete);
        Serial.println();
        if (url == "/pair-setup") {
            onPairSetup(plainBuf);
        } else if (url == "/pair-verify") {
            onPairVerify(plainBuf);
        } else {
        }
    } else {
        Serial.println("received: Method=" + methodStr + " URL=" + url + " complete=");
        Serial.println(complete);
        Serial.println();
    }

    free(data);
    return true;
}

void HKClient::reply(byte *message, size_t messageSize) {
    if (encrypted) {
        sendEncrypted(message, messageSize);
    } else {
        write(message, messageSize);
    }
}

byte *HKClient::receivedDecrypted(size_t &decryptedSize) {
    if (!encrypted) {
        return nullptr;
    }

    size_t encryptedSize = available();
    byte encryptedMessage[encryptedSize];
    readBytes(encryptedMessage, encryptedSize);

    const size_t blockSize = 1024 + 16 + 2;
    decryptedSize = encryptedSize / blockSize * 1024 + encryptedSize % blockSize - 16 - 2;
    auto decrypted = (byte *) malloc(decryptedSize);

    byte nonce[12];
    memset(nonce, 0, 12);

    size_t payloadOffset = 0;
    size_t decryptedOffset = 0;

    while (payloadOffset < encryptedSize) {
        size_t chunkSize = encryptedMessage[payloadOffset] + encryptedMessage[payloadOffset + 1]*256;
        if (chunkSize + 18 > encryptedSize - payloadOffset) {
            // Unfinished chunk
            break;
        }

        byte i = 4;
        int x = countWrites++;
        while (x) {
            nonce[i++] = x % 256;
            x /= 256;
        }

        size_t decryptedLen = decryptedSize - decryptedOffset;
        if (!crypto_verifyAndDecryptAAD(writeKey, nonce, encryptedMessage + payloadOffset, 2, encryptedMessage + payloadOffset + 2, chunkSize, decrypted, encryptedMessage + payloadOffset + 2 + chunkSize)) {
            Serial.println("Could not verify");
            return nullptr;
        }

        decryptedOffset += decryptedLen;
        payloadOffset += chunkSize + 18;
    }

    return decrypted;
}

void HKClient::sendEncrypted(byte *message, size_t messageSize) {
    if (!encrypted || !message || !messageSize) {
        return;
    }
    byte nonce[12];
    memset(nonce, 0, 12);

    size_t payloadOffset = 0;

    while (payloadOffset < messageSize) {
        size_t chunkSize = messageSize - payloadOffset;
        if (chunkSize > 1024) {
            chunkSize = 1024;
        }

        byte aead[2] = { static_cast<byte>(chunkSize % 256), static_cast<byte>(chunkSize / 256) };

        byte encryptedMessage[2 + chunkSize + 16];
        memcpy(encryptedMessage, aead, 2);

        byte i = 4;
        int x = countReads++;
        while (x) {
            nonce[i++] = x % 256;
            x /= 256;
        }

        crypto_encryptAndSealAAD(readKey, nonce, aead, 2, message + payloadOffset, chunkSize, encryptedMessage + 2, encryptedMessage + 2 + chunkSize);
        payloadOffset += chunkSize;
        write(encryptedMessage, sizeof(encryptedMessage));
    }
}

void HKClient::sendTLVResponse(std::vector<HKTLV *> &message) {
    std::vector<byte> payload = HKTLV::formatTLV(message);

    String httpHeaders = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/pairing+tlv8\r\n"
                         "Content-Length: " +
                         String(payload.size()) +
                         "\r\n"
                         "Connection: keep-alive\r\n\r\n";

    uint8_t response[httpHeaders.length() + payload.size()];

    memcpy(response, httpHeaders.c_str(), httpHeaders.length());
    memcpy(response + httpHeaders.length(), &payload[0], payload.size());

    reply(response, httpHeaders.length() + payload.size());
}

void HKClient::sendTLVError(byte state, TLVError error) {
    std::vector<HKTLV *> message = {
            new HKTLV(TLVTypeState, state, 1),
            new HKTLV(TLVTypeError, error, 1)
    };

    sendTLVResponse(message);
}

void HKClient::hkdf(byte *target, byte *ikm, uint8_t ikmLength, byte *salt, uint8_t saltLength, byte *info,
                    uint8_t infoLength) {
    byte prk[64];
    SHA512 sha = SHA512();
    sha.resetHMAC(salt, saltLength);
    sha.update(ikm, ikmLength);
    sha.finalizeHMAC(salt, saltLength, prk, 64);

    sha.reset();
    sha.clear();
    sha.resetHMAC(prk, 64);
    sha.update(info, infoLength);
    sha.finalizeHMAC(prk, 64, target, 32);
}

void HKClient::onPairSetup(const std::vector<byte> &body) {
    std::vector<HKTLV *> message = HKTLV::parseTLV(body);

    uint32_t value = -1;
    HKTLV *state = HKTLV::findTLV(message, TLVTypeState);
    if (state) {
        value = 0;
        for (int j = state->getSize()-1; j >= 0; j--) {
            value = (value << 8) + state->getValue()[j];
        }
    }

    switch (value) {
        case 1: {
            Serial.println("Setup Step 1/3");
            if (HKStorage::isPaired()) {
                Serial.println("Refuse to pair: Already paired");

                sendTLVError(2, TLVErrorUnavailable);
                server->pairing = false;
                break;
            }

            if (server->pairing) {
                Serial.println("Refuse to pair: another pairing in process");

                sendTLVError(2, TLVErrorBusy);
                break;
            } else {
                server->pairing = true;
            }

            srp_start();

            for (auto it = message.begin(); it != message.end();) {
                delete *it;
                it = message.erase(it);
            }
            message.push_back(new HKTLV(TLVTypeState, 2, 1));
            message.push_back(new HKTLV(TLVTypePublicKey, srp_getB(), 384));
            message.push_back(new HKTLV(TLVTypeSalt, srp_getSalt(), 16));

            sendTLVResponse(message);
        }
            break;
        case 3: {
            Serial.println("Setup Step 2/3");
            HKTLV* publicKey = HKTLV::findTLV(message, TLVTypePublicKey);
            HKTLV* proof = HKTLV::findTLV(message, TLVTypeProof);
            if (!publicKey || !proof) {
                Serial.println("Could not find Public Key or Proof in Message");
                sendTLVError(4, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            srp_setA(publicKey->getValue(), publicKey->getSize(), nullptr);

            if (srp_checkM1(proof->getValue(), proof->getSize())) {
                for (auto it = message.begin(); it != message.end();) {
                    delete *it;
                    it = message.erase(it);
                }
                message.push_back(new HKTLV(TLVTypeState, 4, 1));
                message.push_back(new HKTLV(TLVTypeProof, srp_getM2(), 64));

                sendTLVResponse(message);
            } else {
                //return error
                Serial.println("SRP Error");
                sendTLVError(4, TLVErrorAuthentication);
                server->pairing = false;
            }
        }
            break;
        case 5: {
            Serial.println("Setup Step 3/3");

            byte sharedSecret[32];
            const char salt1[] = "Pair-Setup-Encrypt-Salt";
            const char info1[] = "Pair-Setup-Encrypt-Info\001";
            hkdf(sharedSecret, srp_getK(), 64, (uint8_t *) salt1, sizeof(salt1)-1, (uint8_t *) info1, sizeof(info1)-1);

            HKTLV *encryptedTLV = HKTLV::findTLV(message, TLVTypeEncryptedData);
            if (!encryptedTLV) {
                Serial.println("Failed: Could not find Encrypted Data");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            size_t decryptedDataSize = encryptedTLV->getSize() - 16;
            byte decryptedData[decryptedDataSize];

            if (!crypto_verifyAndDecrypt(sharedSecret, (byte *) "PS-Msg05", encryptedTLV->getValue(), decryptedDataSize, decryptedData, encryptedTLV->getValue() + decryptedDataSize)) {
                Serial.println("Decryption failed: MAC not equal");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            std::vector<HKTLV *> decryptedMessage = HKTLV::parseTLV(decryptedData, decryptedDataSize);
            HKTLV *deviceId = HKTLV::findTLV(decryptedMessage, TLVTypeIdentifier);
            if (!deviceId) {
                Serial.println("Decryption failed: Device ID not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            HKTLV *publicKey = HKTLV::findTLV(decryptedMessage, TLVTypePublicKey);
            if (!publicKey) {
                Serial.println("Decryption failed: Public Key not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            HKTLV *signature = HKTLV::findTLV(decryptedMessage, TLVTypeSignature);
            if (!signature) {
                Serial.println("Decryption failed: Signature not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            byte deviceX[32];
            const char salt2[] = "Pair-Setup-Controller-Sign-Salt";
            const char info2[] = "Pair-Setup-Controller-Sign-Info\001";
            hkdf(deviceX, srp_getK(), 64, (uint8_t *) salt2, sizeof(salt2)-1, (uint8_t *) info2, sizeof(info2)-1);

            uint64_t deviceInfoSize = sizeof(deviceX) + deviceId->getSize() + publicKey->getSize();
            byte deviceInfo[deviceInfoSize];
            memcpy(deviceInfo, deviceX, sizeof(deviceX));
            memcpy(deviceInfo + sizeof(deviceX), deviceId->getValue(), deviceId->getSize());
            memcpy(deviceInfo + sizeof(deviceX) + deviceId->getSize(), publicKey->getValue(), publicKey->getSize());

            if (!Ed25519::verify(signature->getValue(), publicKey->getValue(), deviceInfo, deviceInfoSize)) {
                Serial.println("Could not verify Ed25519 Device Info, Signature and Public Key");
                sendTLVError(6, TLVErrorAuthentication);
                server->pairing = false;
                break;
            }

            int result = HKStorage::addPairing((char *) deviceId->getValue(), publicKey->getValue(), 1);
            if (result) {
                Serial.println("COULD NOT STORE PAIRING");
            }

            // M6 Response Generation
            String accessoryId = server->hk->storage->getAccessoryId();
            size_t accessoryInfoSize = 32 + accessoryId.length() + 32;
            byte accessoryInfo[accessoryInfoSize];
            const char salt3[] = "Pair-Setup-Accessory-Sign-Salt";
            const char info3[] = "Pair-Setup-Accessory-Sign-Info\001";
            hkdf(accessoryInfo, srp_getK(), 64, (uint8_t *) salt3, sizeof(salt3)-1, (uint8_t *) info3, sizeof(info3)-1);

            memcpy(accessoryInfo + 32, accessoryId.c_str(), accessoryId.length());
            memcpy(accessoryInfo + 32 + accessoryId.length(), server->hk->storage->getAccessoryKey().publicKey, 32);

            byte accessorySignature[64];
            Ed25519::sign(accessorySignature, server->hk->storage->getAccessoryKey().privateKey, server->hk->storage->getAccessoryKey().publicKey, accessoryInfo, accessoryInfoSize);

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeIdentifier, (byte *) accessoryId.c_str(), accessoryId.length()),
                    new HKTLV(TLVTypePublicKey, server->hk->storage->getAccessoryKey().publicKey, 32),
                    new HKTLV(TLVTypeSignature, accessorySignature, 64)
            };
            std::vector<byte> responseData = HKTLV::formatTLV(responseMessage);

            byte encryptedResponseData[responseData.size() + 16];
            crypto_encryptAndSeal(sharedSecret, (uint8_t *) "PS-Msg06", responseData.data(), responseData.size(), encryptedResponseData, encryptedResponseData + responseData.size());

            for (auto it = message.begin(); it != message.end();) {
                delete *it;
                it = message.erase(it);
            }
            message.push_back(new HKTLV(TLVTypeState, 6, 1));
            message.push_back(new HKTLV(TLVTypeEncryptedData, encryptedResponseData, responseData.size() + 16));

            sendTLVResponse(message);

            server->setupMDNS();

            server->pairing = false;
            Serial.println("Finished Pairing");
        }
            break;
        default:
            break;
    }
}

void HKClient::onPairVerify(const std::vector<byte> &body) {
    std::vector<HKTLV *> message = HKTLV::parseTLV(body);

    int value = -1;
    HKTLV *state = HKTLV::findTLV(message, TLVTypeState);
    if (state) {
        value = 0;
        for (int i = state->getSize()-1; i >= 0; i--) {
            value = (value << 8) + state->getValue()[i];
        }
    }

    switch (value) {
        case 1: {
            verifyContext = new VerifyContext();

            HKTLV *deviceKeyTLV = HKTLV::findTLV(message, TLVTypePublicKey);

            if (!deviceKeyTLV) {
                Serial.println("Device Key not Found");
                break;
            }
            memcpy(verifyContext->devicePublicKey, deviceKeyTLV->getValue(), 32);

            os_get_random(verifyContext->accessorySecretKey, 32);

            crypto_scalarmult_curve25519_base(verifyContext->accessoryPublicKey, verifyContext->accessorySecretKey);
            crypto_scalarmult_curve25519(verifyContext->sharedKey, verifyContext->accessorySecretKey, verifyContext->devicePublicKey);

            String accessoryId = server->hk->storage->getAccessoryId();
            size_t accessoryInfoSize = 32 + accessoryId.length() + 32;
            byte accessoryInfo[accessoryInfoSize];
            memcpy(accessoryInfo, verifyContext->accessoryPublicKey, 32);
            memcpy(accessoryInfo + 32, accessoryId.c_str(), accessoryId.length());
            memcpy(accessoryInfo + 32 + accessoryId.length(), verifyContext->devicePublicKey, 32);

            byte accessorySignature[64];
            Ed25519::sign(accessorySignature, server->hk->storage->getAccessoryKey().privateKey, server->hk->storage->getAccessoryKey().publicKey, accessoryInfo, accessoryInfoSize);

            std::vector<HKTLV *> subResponseMessage = {
                    new HKTLV(TLVTypeIdentifier, (byte *) accessoryId.c_str(), accessoryId.length()),
                    new HKTLV(TLVTypeSignature, accessorySignature, 64)
            };
            std::vector<byte> responseData = HKTLV::formatTLV(subResponseMessage);

            const char salt1[] = "Pair-Verify-Encrypt-Salt";
            const char info1[] = "Pair-Verify-Encrypt-Info\001";
            hkdf(verifyContext->sessionKey, verifyContext->sharedKey, 32, (byte *) salt1, sizeof(salt1)-1, (byte *) info1, sizeof(info1)-1);

            byte encryptedResponseData[responseData.size() + 16];
            crypto_encryptAndSeal(verifyContext->sessionKey, (byte *) "PV-Msg02", responseData.data(), responseData.size(), encryptedResponseData, encryptedResponseData + responseData.size());

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeState, 2, 1),
                    new HKTLV(TLVTypePublicKey, verifyContext->accessoryPublicKey, 32),
                    new HKTLV(TLVTypeEncryptedData, encryptedResponseData, responseData.size() + 16)
            };

            sendTLVResponse(responseMessage);

            Serial.println("Step 1 complete");
            break;
        }
        case 3: {
            Serial.println("Verify Step 2/2");

            HKTLV *encryptedData = HKTLV::findTLV(message, TLVTypeEncryptedData);
            if (!encryptedData) {
                Serial.println("Could not find encrypted data");
                break;
            }

            size_t decryptedDataSize = encryptedData->getSize() - 16;
            byte decryptedData[decryptedDataSize];
            if (!crypto_verifyAndDecrypt(verifyContext->sessionKey, (byte *) "PV-Msg03", encryptedData->getValue(), decryptedDataSize, decryptedData, encryptedData->getValue() + decryptedDataSize)) {
                Serial.println("Could not verify message");
                break;
            }

            std::vector<HKTLV *> decryptedMessage = HKTLV::parseTLV(decryptedData, decryptedDataSize);
            HKTLV *deviceId = HKTLV::findTLV(decryptedMessage, TLVTypeIdentifier);
            if (!deviceId) {
                Serial.println("Could not find device ID");
                break;
            }

            HKTLV *deviceSignature = HKTLV::findTLV(decryptedMessage, TLVTypeSignature);
            if (!deviceSignature) {
                Serial.println("Could not find device Signature");
                break;
            }

            Pairing *pairingItem = HKStorage::findPairing((char *) deviceId->getValue());
            if (!pairingItem) {
                Serial.println("Device is not paired");
                break;
            }

            size_t deviceInfoSize = sizeof(verifyContext->devicePublicKey) + sizeof(verifyContext->accessoryPublicKey) + deviceId->getSize();
            byte deviceInfo[deviceInfoSize];
            memcpy(deviceInfo, verifyContext->devicePublicKey, sizeof(verifyContext->devicePublicKey));
            memcpy(deviceInfo + sizeof(verifyContext->devicePublicKey), deviceId->getValue(), deviceId->getSize());
            memcpy(deviceInfo + sizeof(verifyContext->devicePublicKey) + deviceId->getSize(), verifyContext->accessoryPublicKey, sizeof(verifyContext->accessoryPublicKey));

            if (!Ed25519::verify(deviceSignature->getValue(), pairingItem->deviceKey, deviceInfo, deviceInfoSize)) {
                Serial.println("Could not verify device readInfo");
                break;
            }

            const byte salt[] = "Control-Salt";
            const byte readInfo[] = "Control-Read-Encryption-Key\001";
            hkdf(readKey, verifyContext->sharedKey, 32, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) readInfo, sizeof(readInfo)-1);

            const byte writeInfo[] = "Control-Write-Encryption-Key\001";
            hkdf(writeKey, verifyContext->sharedKey, 32, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) writeInfo, sizeof(writeInfo)-1);

            pairingId = pairingItem->id;
            permission = pairingItem->permissions;

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeState, 4, 1)
            };
            sendTLVResponse(responseMessage);

            delete verifyContext;
            verifyContext = nullptr;
            encrypted = true;
        }
            break;
        default:
            Serial.println("Invalid State");
            break;
    }
}
