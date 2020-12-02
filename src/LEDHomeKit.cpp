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
    auto accessory = new LEDAccessory("Max LED Strip", "LEDs", "2.0");
    hk->setAccessory(accessory);

    HKLOGINFO("[LEDHomeKit::setup] starting WiFi setup name: %s\r\n", hk->getName().c_str());
    wiFiSetup = new WiFiSetup(HKStorage::getSSID(), HKStorage::getWiFiPassword(), hk->getName(), std::bind(&LEDHomeKit::handleSSIDChange, this, std::placeholders::_1, std::placeholders::_2));
    wiFiSetup->start();

    hk->setup();
}

void LEDHomeKit::update() {
    hk->update();
    wiFiSetup->update();
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
