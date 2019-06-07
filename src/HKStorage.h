//
// Created by Max Vissing on 2019-04-27.
//

#ifndef HAP_SERVER_HKSTORAGE_H
#define HAP_SERVER_HKSTORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Ed25519.h>

#ifndef STORAGE_BASE_ADDR
#define STORAGE_BASE_ADDR 0x0
#endif

#define MAGIC_OFFSET           0
#define ACCESSORY_ID_OFFSET    4

#define MAGIC_ADDR           (STORAGE_BASE_ADDR + MAGIC_OFFSET)
#define ACCESSORY_ID_ADDR    (STORAGE_BASE_ADDR + ACCESSORY_ID_OFFSET)

#define MAX_PAIRINGS 16

#define ACCESSORY_ID_SIZE   17

const char magic1[] = "HAP";

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

class HKStorage {
public:
    HKStorage();
    void reset();
    void resetPairings();
    void save();
    void setSSID(const String &ssid);
    String getSSID();
    void setPassword(const String &password);
    String getPassword();
    String getAccessoryId();
    KeyPair getAccessoryKey();

    static bool isPaired();
    static int addPairing(const char *deviceId, const byte *deviceKey, byte permission);
    static Pairing *findPairing(const char *deviceId);
    static int updatePairing(const String &deviceId, byte permission);
    static int removePairing(const String &deviceId);
private:
    static int findEmptyBlock();
    void load();
    String generateAccessoryId();
    KeyPair generateAccessoryKey();
private:
    struct StorageData {
        char ssid[33];
        char password[65];
        char accessoryId[ACCESSORY_ID_SIZE];
        KeyPair accessoryKey;
    };
    struct PairingData {
        char magic[sizeof(magic1)];
        unsigned char permissions;
        char deviceId[36];
        byte devicePublicKey[32];
    };
    StorageData data;
};


#endif //HAP_SERVER_HKSTORAGE_H
