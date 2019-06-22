//
// Created by Max Vissing on 2019-06-01.
//

#include "HomeKit.h"
#include "../lib/crypto/srp.h"

HomeKit::HomeKit(String password, String name) : storage(new HKStorage()), server(new HKServer(this)), accessory(nullptr), password(std::move(password)), name(std::move(name)), configNumber(1) {
}

HomeKit::~HomeKit() {
    delete server;
    delete storage;
}

void HomeKit::setup() {
    HKLOGINFO("[HomeKit::setup] AccessoryID: %s\r\n", storage->getAccessoryId().c_str());

    srp_init((uint8_t *) password.c_str());

    HKLOGINFO("[HomeKit::setup] Password: %s\r\n", (char *) srp_pinMessage() + 11);

    accessory->setup();

    server->setup();
}

void HomeKit::update() {
    server->update();
    accessory->run();
}

void HomeKit::saveSSID(const String& ssid, const String& wiFiPassword) {
    storage->setSSID(ssid);
    storage->setPassword(wiFiPassword);
    storage->save();
}

String HomeKit::getSSID() {
    return storage->getSSID();
}

String HomeKit::getWiFiPassword() {
    return storage->getPassword();
}

String HomeKit::getName() {
    if (name == "") {
        if (accessory) {
            HKService *info = accessory->getService(HKServiceAccessoryInfo);
            if (!info) {
                return generateCustomName();
            }

            HKCharacteristic *serviceName = info->getCharacteristic(HKCharacteristicServiceName);
            if (!serviceName) {
                return generateCustomName();
            }

            name = serviceName->getValue().stringValue;
            String accessoryId = storage->getAccessoryId();
            name += "-" + accessoryId.substring(0, 2) + accessoryId.substring(3, 5);
            return name;
        }
        return generateCustomName();
    }
    return name;
}

String HomeKit::generateCustomName() {
    name = "MyHomeKit-" + storage->getAccessoryId();
    return name;
}

void HomeKit::setAccessory(HKAccessory *accessory) {
    if (accessory->getId() == 1) {
        HomeKit::accessory = accessory;
    }
}

String HomeKit::getAccessoryId() {
    return storage->getAccessoryId();
}

HKAccessory *HomeKit::getAccessory() {
    return accessory;
}

int HomeKit::getConfigNumber() {
    return configNumber;
}
