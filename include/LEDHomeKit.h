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

#ifdef ALEXA_SUPPORT
PROGMEM const char HKALEXA_TCP_HEADERS[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n\r\n";
PROGMEM const char HKALEXA_TCP_STATE_RESPONSE[] = "["
    "{\"success\":{\"/lights/%d/state/on\":%s}},"
    "{\"success\":{\"/lights/%d/state/bri\":%d}}"   // not needed?
    "]";
PROGMEM const char HKALEXA_DEVICE_JSON_TEMPLATE[] = "{"
        "\"type\":\"%s\","
        "\"name\":\"%s\","
        "\"uniqueid\":\"%s\","
        "\"modelid\":\"%s\","
        "\"manufacturername\":\"Philips\","
        "\"productname\":\"E%u\","
        "\"state\":{"
            "\"on\":%s,\"bri\":%d,%s\"alert\":\"none\",%s\"mode\":\"homeautomation\",\"reachable\": true"
        "},"
        "\"swversion\":\"espalexa-2.4.6\""
    "}";
PROGMEM const char HKALEXA_DEVICE_JSON_COLOR_TEMPLATE[] = "\"hue\":%u,"
    "\"sat\":%u,"
    "\"effect\":\"none\","
    "\"xy\":[0.0,0.0],";
#endif

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
