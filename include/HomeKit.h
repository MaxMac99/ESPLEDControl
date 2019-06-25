//
// Created by Max Vissing on 2019-06-01.
//

#ifndef HAP_SERVER_HOMEKIT_H
#define HAP_SERVER_HOMEKIT_H

#ifndef HKLOGLEVEL
#define HKLOGLEVEL 4 // 0: DEBUG, 1: INFO, 2: WARNING, 3: ERROR, 4: NONE
#endif

#if HKLOGLEVEL == 0
#define HKLOGDEBUG(str, ...) Serial.printf("[HomeKit] [DEBUG] " str, ## __VA_ARGS__)
#define HKLOGDEBUGSINGLE(...) Serial.printf(__VA_ARGS__)
#else
#define HKLOGDEBUG(...)
#define HKLOGDEBUGSINGLE(...)
#endif

#if HKLOGLEVEL <= 1
#define HKLOGINFO(str, ...) Serial.printf("[HomeKit] [INFO ] " str, ## __VA_ARGS__)
#else
#define HKLOGINFO(...)
#endif

#if HKLOGLEVEL <= 2
#define HKLOGWARNING(str, ...) Serial.printf("[HomeKit] [WARN ] " str, ## __VA_ARGS__)
#else
#define HKLOGWARNING(...)
#endif

#if HKLOGLEVEL <= 3
#define HKLOGERROR(str, ...) Serial.printf("[HomeKit] [ERROR] " str, ## __VA_ARGS__)
#else
#define HKLOGERROR(...)
#endif

#include <Arduino.h>
#include "HKStorage.h"
#include "HKAccessory.h"
#include "HKServer.h"

class HKAccessory;
class HKServer;

class HomeKit {
public:
    explicit HomeKit(String password, String setupId="", String name="");
    ~HomeKit();
    void setup();
    void update();
    void reset();

    void saveSSID(const String& ssid, const String& wiFiPassword="");
    String getSSID();
    String getWiFiPassword();
    String getName();

    void setAccessory(HKAccessory *accessory);
    HKAccessory *getAccessory();

    String getAccessoryId();
    int getConfigNumber();

    friend class HKClient;
    friend class HKServer;
private:
    String generateCustomName();
private:
    HKStorage *storage;
    HKServer *server;
    HKAccessory *accessory;

    String password;
    String setupId;
    String name;
    int configNumber;
};


#endif //HAP_SERVER_HOMEKIT_H
