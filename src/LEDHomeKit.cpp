//
// Created by Max Vissing on 2019-06-01.
//

#include "LEDHomeKit.h"

LEDHomeKit::LEDHomeKit(String password, String setupId) : hk(new HomeKit(std::move(password), setupId)), wiFiSetup(nullptr) {
}

LEDHomeKit::~LEDHomeKit() {
    delete hk;
    delete wiFiSetup;
}

void LEDHomeKit::setup() {
    auto accessory = new LEDAccessory();
    accessory->addInfoService("MaxLedStrip", "MaxMac Co.", "LEDs", "1234567", "1.0.1");
    hk->setAccessory(accessory);

    wiFiSetup = new WiFiSetup(hk->getSSID(), hk->getWiFiPassword(), hk->getName(), std::bind(&LEDHomeKit::handleSSIDChange, this, std::placeholders::_1, std::placeholders::_2));
    wiFiSetup->start();

    hk->setup();
}

void LEDHomeKit::update() {
    hk->update();
    wiFiSetup->update();
}

void LEDHomeKit::handleSSIDChange(const String &ssid, const String &password) {
    hk->saveSSID(ssid, password);
}
