//
// Created by Max Vissing on 2019-06-01.
//

#include "HomeKit.h"
#include "../lib/crypto/srp.h"

HomeKit::HomeKit(String password, String name) : storage(new HKStorage()), server(new HKServer(this)), accessory(nullptr), password(std::move(password)), name(std::move(name)), aid(0), configNumber(1) {
}

HomeKit::~HomeKit() {
    delete server;
    delete storage;
}

void HomeKit::setup() {
    Serial.println("AccessoryID: " + storage->getAccessoryId());
    storage->resetPairings();

    srp_init((uint8_t *) password.c_str());

    Serial.println("Password: " + String((char *) srp_pinMessage()));
    setupAccessory();

    server->setup();
}

void HomeKit::update() {
    server->update();
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
            HKService *info = accessory->getService(ServiceAccessoryInfo);
            if (!info) {
                return generateCustomName();
            }

            HKCharacteristic *serviceName = info->getCharacteristic(CharacteristicServiceName);
            if (!serviceName) {
                return generateCustomName();
            }

            name = serviceName->getValue().StringValue;
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
    HomeKit::accessory = accessory;
}

void HomeKit::setupAccessory() {
    aid = 1;
    if (accessory->getId() >= aid) {
        aid = accessory->getId()+1;
    } else {
        accessory->setId(aid++);
    }

    accessory->setupServices();
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
