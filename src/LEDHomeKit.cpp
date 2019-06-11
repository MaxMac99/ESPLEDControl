//
// Created by Max Vissing on 2019-06-01.
//

#include "LEDHomeKit.h"

LEDHomeKit::LEDHomeKit(String password) : hk(new HomeKit(std::move(password))), wiFiSetup(nullptr) {
}

LEDHomeKit::~LEDHomeKit() {
    delete hk;
    delete wiFiSetup;
}

void LEDHomeKit::setup() {
    auto accessory = new HKAccessory(AccessoryLightbulb);
    accessory->addInfoService("MaxLedStrip", "MaxMac Co.", "LEDs", "1234567", "1.0.1");

    auto onCharacteristic = new HKCharacteristic(HKCharacteristicOn, HKValue::setBool(false), PermissionPairedRead | PermissionPairedWrite, "On", FormatBool);
    onCharacteristic->setGetter(std::bind(&LEDHomeKit::onOnGet, this));
    onCharacteristic->setSetter(std::bind(&LEDHomeKit::onOnSet, this, std::placeholders::_1));

    auto lightbulbService = new HKService(ServiceLightBulb);
    lightbulbService->addCharacteristic(onCharacteristic);
    lightbulbService->setPrimary(true);

    accessory->addService(lightbulbService);
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

void LEDHomeKit::onOnSet(HKValue value) {
    Serial.println("LED On SET: " + String(value.boolValue));
}

HKValue LEDHomeKit::onOnGet() {
    Serial.println("LED On GET");
    return HKValue::setBool(false);
}
