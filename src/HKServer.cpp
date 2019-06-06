//
// Created by Max Vissing on 2019-04-26.
//

#include "HKServer.h"
#include "HKAccessory.h"

HKServer::HKServer(HomeKit *homeKit) : hk(homeKit), server(nullptr), mdnsService(), pairing(false) {
}

void HKServer::setup() {
    setupMDNS();

    server = new WiFiServer(PORT);
    server->begin();
}

void HKServer::update() {
    MDNS.update();

    HKClient *newClient = new HKClient(server->available());
    if (newClient->client) {
        if (clients.size() >= MAX_CLIENTS) {
            return;
        }

        Serial.println("New Client connected");
        Serial.println("Remote IP: " + String(newClient->client.remoteIP().toString()) + ":" + String(newClient->client.remotePort()));

        newClient->client.setTimeout(10000);
        newClient->client.keepAlive(180, 30, 4);
        clients.push_back(newClient);
    } else {
        delete newClient;
    }

    for (auto it = clients.begin(); it != clients.end();) {
        auto client = *it;
        if (client->client.available()) {
            received(client);
        }

        if (!client->client.connected()) {
            Serial.println("Client disconnected");
            delete *it;
            it = clients.erase(it);
        } else {
            it++;
        }
    }
}

int HKServer::setupMDNS() {
    Serial.println("Setup mDNS");
    String uniqueName = hk->getName();

    if (!mdnsService && !MDNS.begin(uniqueName)) {
        Serial.println("Failed to begin mDNS");
        return -1;
    }
    
    const char *service = "hap";
    const char *protocol = "tcp";
    MDNS.setInstanceName(uniqueName);
    if (!mdnsService) {
        mdnsService = MDNS.addService(nullptr, service, protocol, PORT);
        if (!mdnsService) {
            Serial.println("Failed to add service");
            return -1;
        }
    }

    HKService *info = hk->accessory->getService(ServiceAccessoryInfo);
    if (info == nullptr) {
        return -1;
    }
    HKCharacteristic *model = info->getCharacteristic(CharacteristicModelName);
    if (model == nullptr) {
        return -1;
    }

    if (!MDNS.addServiceTxt(service, protocol, "md", model->getValue().StringValue)) {
        Serial.println("Failed to add model");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "pv", "1.0")) {
        Serial.println("Failed to add \"1.0\"");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "id", hk->storage->getAccessoryId())) {
        Serial.println("Failed to add accessoryId");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "c#", String(hk->configNumber))) {
        Serial.println("Failed to add configNumber");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "s#", "1")) {  // State number
        Serial.println("Failed to 1");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "ff", "0")) {  // feature flags
        //   bit 0 - supports HAP pairing. required for all HomeKit accessories
        //   bits 1-7 - reserved
        Serial.println("Failed to ");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "sf", String(hk->paired ? 0 : 1))) {  // status flags
        //   bit 0 - not paired
        //   bit 1 - not configured to join WiFi
        //   bit 2 - problem detected on accessory
        //   bits 3-7 - reserved
        Serial.println("Failed to ");
        return false;
    }
    if (!MDNS.addServiceTxt(service, protocol, "ci", String(hk->accessory->getCategory()))) {
        Serial.println("Failed to ");
        return false;
    }

    // setupId
    return true;
}

static bool readBytesWithTimeout(WiFiClient &client, size_t maxLength, std::vector<byte> &data, int timeout_ms) {
    while (data.size() < maxLength) {
        int tries = timeout_ms;
        size_t avail;
        while (!(avail = client.available()) && tries--) {
            delay(1);
        }
        if (!avail) {
            break;
        }
        if (data.size() + avail > maxLength) {
            avail = maxLength - data.size();
        }
        while (avail--) {
            data.push_back(client.read());
        }
    }
    return data.size() == maxLength;
}

bool HKServer::received(HKClient *client) {
    size_t dataSize;
    byte *data;
    size_t dataPos = 0;
    if (client->encrypted) {
        dataSize = this->decryptedSize(client->client.available());
        data = decrypt(client);
    } else {
        dataSize = client->client.available();
        data = (byte *) malloc(dataSize);
        client->client.readBytes(data, dataSize);
    }

    String complete = "";

    for (size_t i = dataPos; i < dataSize; i++) {
        if (data[i] == '\r') {
            data[i] = 0;
            break;
        }
    }
    String req = String((char *)data);
    dataPos += req.length() + 2;

    complete += req + '\n';

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

            complete += req + '\n';

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

        if (!client->encrypted && plainBuf.size() < contentLength) {
            if (!readBytesWithTimeout(client->client, contentLength, plainBuf, 2000)) {
                Serial.println("Timeout: " + complete);
                return false;
            }
        }

        if (plainBuf.size() < contentLength) {
            Serial.println("Data too short: " + complete);

            free(data);
            return false;
        }


        if (url == "/pair-setup") {
            onPairSetup(plainBuf, client);
        } else if (url == "/pair-verify") {
            onPairVerify(plainBuf, client);
        } else {
            Serial.println("received: Method=" + methodStr + " URL=" + url + " complete=" + complete);
        }
    } else {
        Serial.println("received: Method=" + methodStr + " URL=" + url + " complete=" + complete);
    }

    free(data);
    return true;
}

byte *convertVectorToByte(std::vector<byte> input) {
    auto result = (byte *) malloc(input.size());
    memcpy(result, input.data(), input.size());
    return result;
}

byte *HKServer::decrypt(HKServer::HKClient *client) {
    if (!client || !client->encrypted) {
        return nullptr;
    }

    size_t encryptedSize = client->client.available();
    byte encrypted[encryptedSize];
    client->client.readBytes(encrypted, encryptedSize);

    size_t decryptedSize = this->decryptedSize(encryptedSize);
    auto decrypted = (byte *) malloc(decryptedSize);

    byte nonce[12];
    memset(nonce, 0, 12);

    size_t payloadOffset = 0;
    size_t decryptedOffset = 0;

    while (payloadOffset < encryptedSize) {
        size_t chunkSize = encrypted[payloadOffset] + encrypted[payloadOffset + 1]*256;
        if (chunkSize + 18 > encryptedSize - payloadOffset) {
            // Unfinished chunk
            break;
        }

        byte i = 4;
        int x = client->countWrites++;
        Serial.println("x: " + String(x));
        while (x) {
            nonce[i++] = x % 256;
            x /= 256;
        }

        size_t decryptedLen = decryptedSize - decryptedOffset;
        if (!crypto_verifyAndDecryptAAD(client->writeKey, nonce, encrypted + payloadOffset, 2, encrypted + payloadOffset + 2, chunkSize, decrypted, encrypted + payloadOffset + 2 + chunkSize)) {
            Serial.println("Could not verify");
            return nullptr;
        }

        decryptedOffset += decryptedLen;
        payloadOffset += chunkSize + 18;
    }

    return decrypted;
}

size_t HKServer::decryptedSize(size_t payloadSize) {
    const size_t blockSize = 1024 + 16 + 2;
    return payloadSize / blockSize * 1024 + payloadSize % blockSize - 16 - 2;
}

void HKServer::onPairSetup(const std::vector<byte> &body, HKClient *client) {
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
            if (hk->paired) {
                Serial.println("Refuse to pair: Already paired");

                sendTLVError(2, TLVErrorUnavailable, client);
                break;
            }

            if (pairing) {
                Serial.println("Refuse to pair: another pairing in process");

                sendTLVError(2, TLVErrorBusy, client);
                break;
            } else {
                pairing = true;
            }

            srp_start();

            for (auto it = message.begin(); it != message.end();) {
                delete *it;
                it = message.erase(it);
            }
            message.push_back(new HKTLV(TLVTypeState, 2, 1));
            message.push_back(new HKTLV(TLVTypePublicKey, srp_getB(), 384));
            message.push_back(new HKTLV(TLVTypeSalt, srp_getSalt(), 16));

            sendTLVResponse(message, client);
        }
        break;
        case 3: {
            Serial.println("Setup Step 2/3");
            HKTLV* publicKey = HKTLV::findTLV(message, TLVTypePublicKey);
            HKTLV* proof = HKTLV::findTLV(message, TLVTypeProof);
            if (!publicKey || !proof) {
                Serial.println("Could not find Public Key or Proof in Message");
                sendTLVError(4, TLVErrorAuthentication, client);
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

                sendTLVResponse(message, client);
            } else {
                //return error
                Serial.println("SRP Error");
                sendTLVError(4, TLVErrorAuthentication, client);
            }
        }
        break;
        case 5: {
            Serial.println("Setup Step 3/3");

            byte sharedSecret[32];
            const char salt1[] = "Pair-Setup-Encrypt-Salt";
            const char info1[] = "Pair-Setup-Encrypt-Info\001";
            //crypto_hkdf(sharedSecret, (uint8_t *) salt1, sizeof(salt1)-1, (uint8_t *) info1, sizeof(info1)-1, srp_getK(), 64);
            hkdf(sharedSecret, srp_getK(), 64, (uint8_t *) salt1, sizeof(salt1)-1, (uint8_t *) info1, sizeof(info1)-1);

            HKTLV *encryptedTLV = HKTLV::findTLV(message, TLVTypeEncryptedData);
            if (!encryptedTLV) {
                Serial.println("Failed: Could not find Encrypted Data");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            size_t decryptedDataSize = encryptedTLV->getSize() - 16;
            byte decryptedData[decryptedDataSize];

            if (!crypto_verifyAndDecrypt(sharedSecret, (byte *) "PS-Msg05", encryptedTLV->getValue(), decryptedDataSize, decryptedData, encryptedTLV->getValue() + decryptedDataSize)) {
                Serial.println("Decryption failed: MAC not equal");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            std::vector<HKTLV *> decryptedMessage = HKTLV::parseTLV(decryptedData, decryptedDataSize);
            HKTLV *deviceId = HKTLV::findTLV(decryptedMessage, TLVTypeIdentifier);
            if (!deviceId) {
                Serial.println("Decryption failed: Device ID not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            HKTLV *publicKey = HKTLV::findTLV(decryptedMessage, TLVTypePublicKey);
            if (!publicKey) {
                Serial.println("Decryption failed: Public Key not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            HKTLV *signature = HKTLV::findTLV(decryptedMessage, TLVTypeSignature);
            if (!signature) {
                Serial.println("Decryption failed: Signature not found in decrypted Message");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            byte deviceX[32];
            const char salt2[] = "Pair-Setup-Controller-Sign-Salt";
            const char info2[] = "Pair-Setup-Controller-Sign-Info\001";
            //crypto_hkdf(deviceX, (uint8_t *) salt2, sizeof(salt2)-1, (uint8_t *) info2, sizeof(info2)-1, srp_getK(), 64);
            hkdf(deviceX, srp_getK(), 64, (uint8_t *) salt2, sizeof(salt2)-1, (uint8_t *) info2, sizeof(info2)-1);

            uint64_t deviceInfoSize = sizeof(deviceX) + deviceId->getSize() + publicKey->getSize();
            byte deviceInfo[deviceInfoSize];
            memcpy(deviceInfo, deviceX, sizeof(deviceX));
            memcpy(deviceInfo + sizeof(deviceX), deviceId->getValue(), deviceId->getSize());
            memcpy(deviceInfo + sizeof(deviceX) + deviceId->getSize(), publicKey->getValue(), publicKey->getSize());

            if (!Ed25519::verify(signature->getValue(), publicKey->getValue(), deviceInfo, deviceInfoSize)) {
                Serial.println("Could not verify Ed25519 Device Info, Signature and Public Key");
                sendTLVError(6, TLVErrorAuthentication, client);
                break;
            }

            int result = HKStorage::addPairing((char *) deviceId->getValue(), publicKey->getValue(), 1);
            if (result) {
                Serial.println("COULD NOT STORE PAIRING");
            }

            // M6 Response Generation
            String accessoryId = hk->storage->getAccessoryId();
            size_t accessoryInfoSize = 32 + accessoryId.length() + 32;
            byte accessoryInfo[accessoryInfoSize];
            const char salt3[] = "Pair-Setup-Accessory-Sign-Salt";
            const char info3[] = "Pair-Setup-Accessory-Sign-Info\001";
            //crypto_hkdf(accessoryInfo, (uint8_t *) salt3, sizeof(salt3)-1, (uint8_t *) info3, sizeof(info3)-1, srp_getK(), 64);
            hkdf(accessoryInfo, srp_getK(), 64, (uint8_t *) salt3, sizeof(salt3)-1, (uint8_t *) info3, sizeof(info3)-1);

            memcpy(accessoryInfo + 32, accessoryId.c_str(), accessoryId.length());
            memcpy(accessoryInfo + 32 + accessoryId.length(), hk->storage->getAccessoryKey().publicKey, 32);

            byte accessorySignature[64];
            Ed25519::sign(accessorySignature, hk->storage->getAccessoryKey().privateKey, hk->storage->getAccessoryKey().publicKey, accessoryInfo, accessoryInfoSize);

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeIdentifier, (byte *) accessoryId.c_str(), accessoryId.length()),
                    new HKTLV(TLVTypePublicKey, hk->storage->getAccessoryKey().publicKey, 32),
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

            sendTLVResponse(message, client);

            hk->paired = true;
            setupMDNS();

            pairing = false;
            Serial.println("Finished Pairing");
        }
        break;
        default:
            break;
    }
}

void HKServer::onPairVerify(const std::vector<byte> &body, HKClient *client) {
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
            client->verifyContext = VerifyContext();

            HKTLV *deviceKeyTLV = HKTLV::findTLV(message, TLVTypePublicKey);

            if (!deviceKeyTLV) {
                Serial.println("Device Key not Found");
                break;
            }
            memcpy(client->verifyContext.devicePublicKey, deviceKeyTLV->getValue(), 32);

            os_get_random(client->verifyContext.accessorySecretKey, 32);

            crypto_scalarmult_curve25519_base(client->verifyContext.accessoryPublicKey, client->verifyContext.accessorySecretKey);
            crypto_scalarmult_curve25519(client->verifyContext.sharedKey, client->verifyContext.accessorySecretKey, client->verifyContext.devicePublicKey);

            String accessoryId = hk->storage->getAccessoryId();
            size_t accessoryInfoSize = 32 + accessoryId.length() + 32;
            byte accessoryInfo[accessoryInfoSize];
            memcpy(accessoryInfo, client->verifyContext.accessoryPublicKey, 32);
            memcpy(accessoryInfo + 32, accessoryId.c_str(), accessoryId.length());
            memcpy(accessoryInfo + 32 + accessoryId.length(), client->verifyContext.devicePublicKey, 32);

            byte accessorySignature[64];
            Ed25519::sign(accessorySignature, hk->storage->getAccessoryKey().privateKey, hk->storage->getAccessoryKey().publicKey, accessoryInfo, accessoryInfoSize);

            std::vector<HKTLV *> subResponseMessage = {
                    new HKTLV(TLVTypeIdentifier, (byte *) accessoryId.c_str(), accessoryId.length()),
                    new HKTLV(TLVTypeSignature, accessorySignature, 64)
            };
            std::vector<byte> responseData = HKTLV::formatTLV(subResponseMessage);

            const char salt1[] = "Pair-Verify-Encrypt-Salt";
            const char info1[] = "Pair-Verify-Encrypt-Info\001";
            //crypto_hkdf(client->verifyContext.sessionKey, (byte *) salt1, sizeof(salt1)-1, (byte *) info1, sizeof(info1)-1, client->verifyContext.sharedKey, 32);
            hkdf(client->verifyContext.sessionKey, client->verifyContext.sharedKey, 32, (byte *) salt1, sizeof(salt1)-1, (byte *) info1, sizeof(info1)-1);

            byte encryptedResponseData[responseData.size() + 16];
            crypto_encryptAndSeal(client->verifyContext.sessionKey, (byte *) "PV-Msg02", responseData.data(), responseData.size(), encryptedResponseData, encryptedResponseData + responseData.size());

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeState, 2, 1),
                    new HKTLV(TLVTypePublicKey, client->verifyContext.accessoryPublicKey, 32),
                    new HKTLV(TLVTypeEncryptedData, encryptedResponseData, responseData.size() + 16)
            };

            sendTLVResponse(responseMessage, client);

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
            if (!crypto_verifyAndDecrypt(client->verifyContext.sessionKey, (byte *) "PV-Msg03", encryptedData->getValue(), decryptedDataSize, decryptedData, encryptedData->getValue() + decryptedDataSize)) {
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

            size_t deviceInfoSize = sizeof(client->verifyContext.devicePublicKey) + sizeof(client->verifyContext.accessoryPublicKey) + deviceId->getSize();
            byte deviceInfo[deviceInfoSize];
            memcpy(deviceInfo, client->verifyContext.devicePublicKey, sizeof(client->verifyContext.devicePublicKey));
            memcpy(deviceInfo + sizeof(client->verifyContext.devicePublicKey), deviceId->getValue(), deviceId->getSize());
            memcpy(deviceInfo + sizeof(client->verifyContext.devicePublicKey) + deviceId->getSize(), client->verifyContext.accessoryPublicKey, sizeof(client->verifyContext.accessoryPublicKey));

            if (!Ed25519::verify(deviceSignature->getValue(), pairingItem->deviceKey, deviceInfo, deviceInfoSize)) {
                Serial.println("Could not verify device readInfo");
                break;
            }

            const byte salt[] = "Control-Salt";
            const byte readInfo[] = "Control-Read-Encryption-Key\001";
            //crypto_hkdf(client->readKey, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) readInfo, sizeof(readInfo)-1, client->verifyContext.sharedKey, 32);
            hkdf(client->readKey, client->verifyContext.sharedKey, 32, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) readInfo, sizeof(readInfo)-1);

            const byte writeInfo[] = "Control-Write-Encryption-Key\001";
            //crypto_hkdf(client->writeKey, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) writeInfo, sizeof(writeInfo)-1, client->verifyContext.sharedKey, 32);
            hkdf(client->writeKey, client->verifyContext.sharedKey, 32, (uint8_t *) salt, sizeof(salt)-1, (uint8_t *) writeInfo, sizeof(writeInfo)-1);

            client->pairingId = pairingItem->id;
            client->permission = pairingItem->permissions;

            std::vector<HKTLV *> responseMessage = {
                    new HKTLV(TLVTypeState, 4, 1)
            };
            sendTLVResponse(responseMessage, client);
            client->verifyContext = {};
            client->encrypted = true;
        }
        break;
        default:
            Serial.println("Invalid State");
            break;
    }
}

void HKServer::sendTLVResponse(std::vector<HKTLV *> &message, HKClient *client) {
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

    client->client.write(response, httpHeaders.length() + payload.size());
}

void HKServer::sendTLVError(byte state, TLVError error, HKClient *client) {
    std::vector<HKTLV *> message = {
            new HKTLV(TLVTypeState, state, 1),
            new HKTLV(TLVTypeError, error, 1)
    };

    sendTLVResponse(message, client);
    pairing = false;
}

void HKServer::hkdf(byte *target, byte *ikm, uint8_t ikmLength, byte *salt, uint8_t saltLength, byte *info,
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
