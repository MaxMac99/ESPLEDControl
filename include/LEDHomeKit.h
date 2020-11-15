//
// Created by Max Vissing on 2019-06-01.
//

#ifndef HAP_SERVER_LEDHOMEKIT_H
#define HAP_SERVER_LEDHOMEKIT_H

#include <Arduino.h>
#include <ESPHomeKit.h>
#include "LEDAccessory.h"
#include <WiFiSetup.h>

class LEDHomeKit {
public:
    explicit LEDHomeKit();
    ~LEDHomeKit();
    void setup();
    void update();
    void handleReset();
private:
    void handleSSIDChange(const String& ssid, const String& password);
private:
    ESPHomeKit *hk;
    WiFiSetup *wiFiSetup;
};


#endif //HAP_SERVER_LEDHOMEKIT_H
