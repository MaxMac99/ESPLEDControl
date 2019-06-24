//
// Created by Max Vissing on 2019-04-27.
//

#include "HKStorage.h"
#include "HomeKit.h"

HKStorage::HKStorage() : data() {
    load();
}

void HKStorage::load() {
    char magic[sizeof(magic1)];
    memset(magic, 0, sizeof(magic));

    EEPROM.begin(4096);
    for (unsigned int i = 0; i < sizeof(magic); i++) {
        magic[i] = EEPROM.read(MAGIC_ADDR + i);
    }
    EEPROM.end();

    if (strncmp(magic, magic1, sizeof(magic1)) != 0) {
        reset();
    }

    EEPROM.begin(4096);
    EEPROM.get(ACCESSORY_ID_ADDR, data);
    EEPROM.end();
}

void HKStorage::save() {
    EEPROM.begin(4096);
    EEPROM.put(ACCESSORY_ID_ADDR, data);
    EEPROM.end();
}

void HKStorage::reset() {
    EEPROM.begin(4096);
    for (unsigned int i = 0; i < sizeof(magic1); i++) {
        EEPROM.write(MAGIC_ADDR + i, magic1[i]);
    }
    for (unsigned int i = ACCESSORY_ID_ADDR; i < 4096; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.get(ACCESSORY_ID_ADDR, data);
    EEPROM.end();
    generateAccessoryId();
    generateAccessoryKey();
    save();
}

void HKStorage::resetPairings() {
    EEPROM.begin(4096);
    for (unsigned int i = ACCESSORY_ID_ADDR + sizeof(StorageData); i < 4096; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
    HKLOGINFO("[HKStorage::resetPairings] Reset Pairings\r\n");
}

String HKStorage::getAccessoryId() {
    if (strcmp(data.accessoryId, "") == 0) {
        return generateAccessoryId();
    } else {
        return strndup(data.accessoryId, ACCESSORY_ID_SIZE);
    }
}

KeyPair HKStorage::getAccessoryKey() {
    KeyPair zero{};
    if (memcmp(data.accessoryKey.privateKey, zero.privateKey, sizeof(data.accessoryKey.privateKey)) == 0 || memcmp(data.accessoryKey.publicKey, zero.publicKey, sizeof(data.accessoryKey.publicKey)) == 0) {
        return generateAccessoryKey();
    }
    return data.accessoryKey;
}

void HKStorage::setSSID(const String &ssid) {
    if (ssid.length() < sizeof(data.ssid)-1) {
        strcpy(data.ssid, ssid.c_str());
    }
}

void HKStorage::setPassword(const String &password) {
    if (password.length() < sizeof(data.password)-1) {
        strcpy(data.password, password.c_str());
    }
}

String HKStorage::getSSID() {
    return String(data.ssid);
}

String HKStorage::getPassword() {
    return String(data.password);
}

String HKStorage::generateAccessoryId() {
    uint32_t randomNumber;
    String accessoryId = String();
    for (uint8_t i = 0; i < 6; i++) {
        randomNumber = random(0xFF);
        String byteToAdd = String(randomNumber, HEX);
        byteToAdd.toUpperCase();
        if (byteToAdd.length() < 2) {
            byteToAdd = '0' + byteToAdd;
        }
        accessoryId += byteToAdd;
        if (i != 5) {
            accessoryId += ':';
        }
    }

    HKLOGINFO("[HKStorage::generateAccessoryId] Accessory ID: %s\r\n", accessoryId.c_str());

    memcpy(data.accessoryId, accessoryId.c_str(), accessoryId.length());
    return accessoryId;
}

KeyPair HKStorage::generateAccessoryKey() {
    HKLOGINFO("[HKStorage::generateAccessoryKey] Generating Accessory Key\r\n");
    os_get_random(data.accessoryKey.privateKey, sizeof(data.accessoryKey.privateKey));
    Ed25519::derivePublicKey(data.accessoryKey.publicKey, data.accessoryKey.privateKey);
    return data.accessoryKey;
}

int HKStorage::findEmptyBlock() {
    unsigned char pairingData[sizeof(PairingData)];
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);

        bool empty = true;
        for (unsigned char j : pairingData) {
            if (j != 0x0) {
                empty = false;
                break;
            }
        }

        if (empty) {
            EEPROM.end();
            return i;
        }
    }

    EEPROM.end();
    return -1;
}

bool HKStorage::isPaired() {
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }
        EEPROM.end();
        return true;
    }
    EEPROM.end();
    return false;
}

int HKStorage::addPairing(const char *deviceId, const byte *deviceKey, byte permission) {
    int nextBlockIdx = findEmptyBlock();
    if (nextBlockIdx == -1) {
        //compactData();
        nextBlockIdx = findEmptyBlock();
    }

    if (nextBlockIdx == -1) {
        //Failed to write pairing info to flash: max number of pairings
        return -2;
    }

    PairingData pairingData{};
    strncpy(pairingData.magic, magic1, sizeof(magic1));
    pairingData.permissions = permission;
    strncpy(pairingData.deviceId, deviceId, sizeof(pairingData.deviceId));
    memcpy(pairingData.devicePublicKey, deviceKey, sizeof(pairingData.devicePublicKey));

    EEPROM.begin(4096);
    EEPROM.put(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*nextBlockIdx, pairingData);
    EEPROM.end();

    return 0;
}

int HKStorage::updatePairing(const String &deviceId, byte permission) {
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }

        if (strncmp(pairingData.deviceId, deviceId.c_str(), sizeof(pairingData.deviceId)) == 0) {
            pairingData.permissions = permission;
            EEPROM.put(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
            EEPROM.end();
            return 0;
        }
    }
    EEPROM.end();
    return -1;
}

Pairing *HKStorage::findPairing(const char *deviceId) {
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }

        if (memcmp(pairingData.deviceId, deviceId, sizeof(pairingData.deviceId)) == 0) {
            auto pairing = new Pairing();
            pairing->id = i;
            strncpy(pairing->deviceId, pairingData.deviceId, sizeof(pairingData.deviceId));
            memcpy(pairing->deviceKey, pairingData.devicePublicKey, sizeof(pairingData.devicePublicKey));
            pairing->permissions = pairingData.permissions;

            EEPROM.end();
            return pairing;
        }
    }
    EEPROM.end();
    return nullptr;
}

int HKStorage::removePairing(const String &deviceId) {
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }

        if (strncmp(pairingData.deviceId, deviceId.c_str(), sizeof(pairingData.deviceId)) == 0) {
            PairingData empty{};
            EEPROM.put(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, empty);
            EEPROM.end();
            return 0;
        }
    }
    EEPROM.end();
    return -1;
}

bool HKStorage::hasPairedAdmin() {
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }

        if (pairingData.permissions & PairingPermissionAdmin) {
            EEPROM.end();
            return true;
        }
    }
    EEPROM.end();
    return false;
}

std::vector<Pairing *> HKStorage::getPairings() {
    std::vector<Pairing *> pairings;
    PairingData pairingData{};
    EEPROM.begin(4096);
    for (int i = 0; i < MAX_PAIRINGS; i++) {
        EEPROM.get(ACCESSORY_ID_ADDR + sizeof(StorageData) + sizeof(pairingData)*i, pairingData);
        if (strncmp(pairingData.magic, magic1, sizeof(magic1)) != 0) {
            continue;
        }

        auto pairing = new Pairing();
        pairing->id = i;
        strncpy(pairing->deviceId, pairingData.deviceId, sizeof(pairingData.deviceId));
        memcpy(pairing->deviceKey, pairingData.devicePublicKey, sizeof(pairingData.devicePublicKey));
        pairing->permissions = pairingData.permissions;
        pairings.push_back(pairing);
    }
    EEPROM.end();
    return pairings;
}
