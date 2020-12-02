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

#include "FastLEDStrip.h"
#include "NeoPixelBusStrip.h"

class LEDMode;

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
    #ifdef ALEXA_SUPPORT
    void serveNotFound();
    bool serveList(String url, String body);
    bool serveControl(String url, String body);
    void serveDescription();
    void respondToAlexaSearch();
    String byteToHex(uint8_t num);
    String makeMD5(String text);
    String deviceToJSON(LEDMode *mode, uint8_t id);
    #endif
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
    ESP8266WebServer *server;
    WiFiUDP alexaUdp;
    #endif
};


#endif //HAP_SERVER_LEDHOMEKIT_H
