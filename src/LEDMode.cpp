//
// Created by Max Vissing on 2019-06-23.
//

#include "LEDMode.h"

LEDMode::LEDMode(std::shared_ptr<CRGBSet> leds, LEDAccessory *accessory, bool primary) : HKService(HKServiceLightBulb, false, primary), accessory(accessory), leds(std::move(leds)) {}

void LEDMode::setupCharacteristics() {
    auto onChar = new HKCharacteristic(HKCharacteristicOn, HKValue(FormatBool, false), PermissionPairedRead | PermissionPairedWrite | PermissionNotify, "On", FormatBool);
    onChar->setSetter(std::bind(&LEDAccessory::setOn, accessory, this, std::placeholders::_1));
    onChar->setGetter(std::bind(&LEDAccessory::getOn, accessory, this));
    addCharacteristic(onChar);

    setup();
}

void LEDMode::turnOn() {
    if (HKCharacteristic *characteristic = getCharacteristic(HKCharacteristicOn)) {
        characteristic->notify(HKValue(FormatBool, true));
    }
    start();
}

void LEDMode::turnOff() {
    if (HKCharacteristic *characteristic = getCharacteristic(HKCharacteristicOn)) {
        characteristic->notify(HKValue(FormatBool, false));
    }
    stop();
}

void LEDMode::addNameCharacteristic(const char *name) {
    auto onChar = new HKCharacteristic(HKCharacteristicName, HKValue(FormatString, name), PermissionPairedRead, "Name", FormatString);
    addCharacteristic(onChar);
}

void LEDMode::addBrightnessCharacteristic() {
    auto brightnessChar = new HKCharacteristic(HKCharacteristicBrightness, HKValue(FormatInt, 0), PermissionPairedRead | PermissionPairedWrite | PermissionNotify, "Brightness", FormatInt, UnitPercentage, new float(0), new float(100), new float(1));
    brightnessChar->setSetter(std::bind(&LEDAccessory::setBrightness, accessory, this, std::placeholders::_1));
    brightnessChar->setGetter(std::bind(&LEDAccessory::getBrightness, accessory, this));
    addCharacteristic(brightnessChar);
}

void LEDMode::addHueCharacteristic() {
    auto hueChar = new HKCharacteristic(HKCharacteristicHue, HKValue(FormatFloat, 0), PermissionPairedRead | PermissionPairedWrite | PermissionNotify, "Hue", FormatFloat, UnitArcdegrees, new float(0), new float(360), new float(1));
    hueChar->setSetter(std::bind(&LEDAccessory::setHue, accessory, this, std::placeholders::_1));
    hueChar->setGetter(std::bind(&LEDAccessory::getHue, accessory, this));
    addCharacteristic(hueChar);
}

void LEDMode::addSaturationCharacteristic() {
    auto saturationChar = new HKCharacteristic(HKCharacteristicSaturation, HKValue(FormatFloat, 0), PermissionPairedRead | PermissionPairedWrite | PermissionNotify, "Saturation", FormatFloat, UnitPercentage, new float(0), new float(100), new float(1));
    saturationChar->setSetter(std::bind(&LEDAccessory::setSaturation, accessory, this, std::placeholders::_1));
    saturationChar->setGetter(std::bind(&LEDAccessory::getSaturation, accessory, this));
    addCharacteristic(saturationChar);
}
