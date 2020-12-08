//
// Created by Max Vissing on 2019-06-01.
//

#ifndef HAP_SERVER_LEDHOMEKIT_H
#define HAP_SERVER_LEDHOMEKIT_H

#include <Arduino.h>
#include <ESPHomeKit.h>
#include "LEDAccessory.h"
#include "LEDMode.h"
#include <WiFiSetup.h>
#include <ArduinoOTA.h>

#include "ESPAlexaLights.h"
#include "FastLEDStrip.h"
#include "NeoPixelBusStrip.h"

class LEDMode;
class ESPAlexaLights;

class LEDHomeKit {
public:
    static LEDHomeKit *shared() {
       static CGuard g;
       if (!_instance) {
          _instance = new LEDHomeKit();
        }
       return _instance;
    };
    void setup();
    void update();
    void handleReset();
    void resetPairings();
    LEDStrip *getStrip();
private:
    void handleSSIDChange(const String& ssid, const String& password);
private:
    static LEDHomeKit *_instance;
    LEDHomeKit();
    LEDHomeKit(const LEDHomeKit &) {};
    ~LEDHomeKit();
    class CGuard {
    public:
        ~CGuard() {
            if (NULL != LEDHomeKit::_instance) {
                delete LEDHomeKit::_instance;
                LEDHomeKit::_instance = NULL;
            }
        }
    };
    ESPHomeKit *hk;
    WiFiSetup *wiFiSetup;
    LEDStrip *strip;
    #ifdef ALEXA_SUPPORT
    ESPAlexaLights *alexa;
    #endif
};


#endif //HAP_SERVER_LEDHOMEKIT_H
