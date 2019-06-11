//
// Created by Max Vissing on 2019-06-01.
//

#ifndef HAP_SERVER_LEDHOMEKIT_H
#define HAP_SERVER_LEDHOMEKIT_H

#include <Arduino.h>
#include "HomeKit.h"
#include <WiFiSetup.h>

class LEDHomeKit {
public:
    explicit LEDHomeKit(String password);
    ~LEDHomeKit();
    void setup();
    void update();
    void onOnSet(HKValue value);
    HKValue onOnGet();
private:
    void handleSSIDChange(const String& ssid, const String& password);
private:
    HomeKit *hk;
    WiFiSetup *wiFiSetup;
};


#endif //HAP_SERVER_LEDHOMEKIT_H
