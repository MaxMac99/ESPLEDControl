//
// Created by Max Vissing on 2019-06-01.
//

#ifndef HAP_SERVER_HOMEKIT_H
#define HAP_SERVER_HOMEKIT_H

#include <Arduino.h>
#include "HKStorage.h"
#include "HKServer.h"
#include "HKAccessory.h"

class HKServer;

class HomeKit {
public:
    explicit HomeKit(String password, String name="");
    ~HomeKit();
    void setup();
    void update();

    void saveSSID(const String& ssid, const String& wiFiPassword="");
    String getSSID();
    String getWiFiPassword();
    String getName();

    void setAccessory(HKAccessory *accessory);
    String getAccessoryId();
    HKAccessory *getAccessory();
    int getConfigNumber();

    friend class HKClient;
private:
    String generateCustomName();
    void setupAccessory();
private:
    HKStorage *storage;
    HKServer *server;
    HKAccessory *accessory;

    String password;
    String name;
    unsigned int aid;
    int configNumber;
};


#endif //HAP_SERVER_HOMEKIT_H
