//
// Created by Max Vissing on 2019-04-27.
//

#ifndef HAP_SERVER_HKSTORAGE_H
#define HAP_SERVER_HKSTORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Ed25519.h>
#include "HomeKit.h"

#ifndef STORAGE_BASE_ADDR
#define STORAGE_BASE_ADDR 0x0
#endif

#define MAGIC_OFFSET           0
#define ACCESSORY_ID_OFFSET    4

#define MAGIC_ADDR           (STORAGE_BASE_ADDR + MAGIC_OFFSET)
#define ACCESSORY_ID_ADDR    (STORAGE_BASE_ADDR + ACCESSORY_ID_OFFSET)

#define MAX_PAIRINGS 16

#define ACCESSORY_ID_SIZE   17

#define COMPARE_SIZE 3

struct Pairing {
    int id;
    char deviceId[36];
    byte deviceKey[32];
    unsigned char permissions;
};

struct KeyPair {
    byte privateKey[32];
    byte publicKey[32];
};

class HomeKit;

class HKStorage {
public:
    HKStorage(HomeKit *hk);
    void reset();
    void resetPairings();
    void save();
    void setSSID(const String &ssid);
    String getSSID();
    void setPassword(const String &password);
    String getPassword();
    String getAccessoryId();
    KeyPair getAccessoryKey();

    bool isPaired();
    bool hasPairedAdmin();
    std::vector<Pairing *> getPairings();
    int addPairing(const char *deviceId, const byte *deviceKey, byte permission);
    Pairing *findPairing(const char *deviceId);
    int updatePairing(const String &deviceId, byte permission);
    int removePairing(const String &deviceId);
private:
    int findEmptyBlock();
    void load();
    String generateAccessoryId();
    KeyPair generateAccessoryKey();
private:
    HomeKit *hk;
    struct StorageData {
        char ssid[33];
        char password[65];
        char accessoryId[ACCESSORY_ID_SIZE];
        KeyPair accessoryKey;
    };
    struct PairingData {
        char magic[COMPARE_SIZE];
        unsigned char permissions;
        char deviceId[36];
        byte devicePublicKey[32];
    };
    StorageData data;
};


#endif //HAP_SERVER_HKSTORAGE_H
