/**
 * @file ESPAlexaLights.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-12-07
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef ALEXA_SUPPORT

#ifndef HAP_SERVER_ALEXA_LIGHTS_H
#define HAP_SERVER_ALEXA_LIGHTS_H

#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESPHomeKit.h>
#include "LEDMode.h"

PROGMEM const char ESPALEXA_TCP_HEADERS[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %S\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n\r\n";
PROGMEM const char ESPALEXA_TCP_HEADERS_WITH_CONTENT[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %S\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n\r\n"
    "%S";
PROGMEM const char ESPALEXA_TCP_STATE_RESPONSE[] = "["
    "{\"success\":{\"/lights/%d/state/on\":%s}},"
    "{\"success\":{\"/lights/%d/state/bri\":%d}}"   // not needed?
    "]";
PROGMEM const char ESPALEXA_DEVICE_JSON_TEMPLATE[] = "{"
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
PROGMEM const char ESPALEXA_DEVICE_JSON_COLOR_TEMPLATE[] = "\"hue\":%u,"
    "\"sat\":%u,"
    "\"effect\":\"none\","
    "\"xy\":[0.0,0.0],";
PROGMEM const char ESPALEXA_DEVICE_TYPE_RESPONSE[] = "["
    "{"
        "\"success\":{"
            "\"username\":\"2WLEDHardQrI3WHYTHoMcXHgEspsM8ZZRpSKtBQr\""
        "}"
    "}]";

class LEDMode;

class ESPAlexaLights {
public:
    ESPAlexaLights(ESPHomeKit *hk);
    ESPAlexaLights(ESPHomeKit *hk, ESP8266WebServer *server);
    ~ESPAlexaLights();
    void begin();
    void handle();
private:
    void serveUDPMessage();
    void serveDescription();
    void serveNotFound();
    bool serveList(const String &url, const String &body);
    bool serveDevice(const String &url, const String &body);
    bool serveControl(const String &url, const String &body);
    bool serveDeviceType();
    String deviceToJSON(LEDMode *mode, uint8_t id);
    String byteToHex(uint8_t num);
    String makeMD5(const String &text);
private:
    ESPHomeKit *hk;
    ESP8266WebServer *server;
    WiFiUDP udpServer;
};

#endif

#endif
