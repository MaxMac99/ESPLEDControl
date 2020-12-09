//
// Created by Max Vissing on 2019-06-01.
//

#include "LEDHomeKit.h"

LEDHomeKit::LEDHomeKit() : hk(new ESPHomeKit()), wiFiSetup(nullptr)
#ifdef FAST_LED
, strip(new FastLEDStrip())
#else
, strip(new NeoPixelBusStrip())
#endif
#ifdef ALEXA_SUPPORT
, alexa(new ESPAlexaLights(hk))
#endif
{
    strip->begin();
}

LEDHomeKit *LEDHomeKit::_instance = 0;

LEDHomeKit::~LEDHomeKit() {
    delete hk;
    delete wiFiSetup;
    delete strip;
}

void LEDHomeKit::setup() {
    strip->clearTo(HSIColor(0, 100, 0));
    strip->show();

    #ifdef HK_NAME
    String name = String(HK_NAME);
    #else
    String name = "Max LED Strip";
    #endif
    auto accessory = new LEDAccessory(name, "LEDs", "2.0");

    // HomeKit
    hk->setup(accessory);

    // Wifi setup
    HKLOGINFO("[LEDHomeKit::setup] starting WiFi setup name: %s\r\n", hk->getName().c_str());
    wiFiSetup = new WiFiSetup(HKStorage::getSSID(), HKStorage::getWiFiPassword(), hk->getName(), std::bind(&LEDHomeKit::handleSSIDChange, this, std::placeholders::_1, std::placeholders::_2));
    wiFiSetup->start();
    HKLOGINFO("[LEDHomeKit::setup] WiFiSetup started\r\n");
    HKLOGINFO("[LEDHomeKit::setup] IP Adress: %s\r\n", WiFi.localIP().toString().c_str());

    hk->begin();
    
    #ifdef ALEXA_SUPPORT
    alexa->begin();
    #endif

    // OTA
    ArduinoOTA.setHostname(hk->getName().c_str());
    ArduinoOTA.setPassword(String(HKPASSWORD).c_str());

    #if HKLOGLEVEL <= 1
    ArduinoOTA.onStart([]() {
        HKLOGINFO("[ArduinoOTA] Start\r\n");
    });
    ArduinoOTA.onEnd([]() {
        HKLOGINFO("[ArduinoOTA] End\r\n");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        HKLOGINFO("[ArduinoOTA] Progress: %u%%\r", (progress / (total / 100)));
    });
    #endif
    #if HKLOGLEVEL <= 3
    ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) HKLOGERROR("[ArduinoOTA] Error[%u]: Auth Failed\r\n", error);
        else if (error == OTA_BEGIN_ERROR) HKLOGERROR("[ArduinoOTA] Error[%u]: Begin Failed\r\n", error);
        else if (error == OTA_CONNECT_ERROR) HKLOGERROR("[ArduinoOTA] Error[%u]: Connect Failed\r\n", error);
        else if (error == OTA_RECEIVE_ERROR) HKLOGERROR("[ArduinoOTA] Error[%u]: Receive Failed\r\n", error);
        else if (error == OTA_END_ERROR) HKLOGERROR("[ArduinoOTA] Error[%u]: End Failed\r\n", error);
    });
    #endif

    ArduinoOTA.begin();

    HKLOGINFO("[LEDHomeKit::setup] Setup finished\r\n");
}

void LEDHomeKit::update() {
    ArduinoOTA.handle();
    hk->update();
    wiFiSetup->update();
    
    #ifdef ALEXA_SUPPORT
    alexa->handle();
    #endif
}

void LEDHomeKit::handleSSIDChange(const String &ssid, const String &password) {
    HKLOGINFO("[LEDHomeKit::handleSSIDChange] change SSID to %s\r\n", ssid.c_str());
    HKStorage::saveSSID(ssid);
    HKStorage::saveWiFiPassword(password);
}

void LEDHomeKit::handleReset() {
    HKLOGINFO("[LEDHomeKit::handleReset] handleReset\r\n");
    hk->reset();
}

void LEDHomeKit::resetPairings() {
    HKLOGINFO("[LEDHomeKit::resetPairings] resetPairings\r\n");
    hk->resetPairings();
}

LEDStrip *LEDHomeKit::getStrip() {
    return strip;
}
