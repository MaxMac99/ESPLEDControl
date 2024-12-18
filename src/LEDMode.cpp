//
// Created by Max Vissing on 2019-06-23.
//

#include "LEDMode.h"

LEDMode::LEDMode(LEDAccessory *accessory, String name, bool primary) :
#ifdef HK_MODE_PREFIX
HKService(HKServiceLightBulb, false, primary, HK_MODE_PREFIX " " + name), 
#else
HKService(HKServiceLightBulb, false, primary, name), 
#endif
accessory(accessory) {}

void LEDMode::setupCharacteristics() {
    auto onChar = new HKCharacteristic(HKCharacteristicOn, HKValue(HKFormatBool, false), HKPermissionPairedRead | HKPermissionPairedWrite | HKPermissionNotify, "On", HKFormatBool);
    onChar->setSetter(std::bind(&LEDAccessory::setOn, accessory, this, std::placeholders::_1));
    onChar->setGetter(std::bind(&LEDAccessory::getOn, accessory, this));
    addCharacteristic(onChar);

    setup();
}

void LEDMode::turnOn(bool cleanStart) {
    if (HKCharacteristic *characteristic = getCharacteristic(HKCharacteristicOn)) {
        characteristic->notify(HKValue(HKFormatBool, true));
    }
    if (getCharacteristic(HKCharacteristicOn) != nullptr && getBrightness() == 0) {
        this->setBrightness(100, true);
    }
    start(cleanStart);
}

void LEDMode::turnOff() {
    if (HKCharacteristic *characteristic = getCharacteristic(HKCharacteristicOn)) {
        characteristic->notify(HKValue(HKFormatBool, false));
    }
    stop();
}

void LEDMode::addBrightnessCharacteristic() {
    auto brightnessChar = new HKCharacteristic(HKCharacteristicBrightness, HKValue(HKFormatInt, 0), HKPermissionPairedRead | HKPermissionPairedWrite | HKPermissionNotify, "Brightness", HKFormatInt, HKUnitPercentage, new float(0), new float(100), new float(1));
    brightnessChar->setSetter(std::bind(&LEDAccessory::setBrightness, accessory, this, std::placeholders::_1));
    brightnessChar->setGetter(std::bind(&LEDAccessory::getBrightness, accessory, this));
    addCharacteristic(brightnessChar);
}

void LEDMode::addHueCharacteristic() {
    auto hueChar = new HKCharacteristic(HKCharacteristicHue, HKValue(HKFormatFloat, 0), HKPermissionPairedRead | HKPermissionPairedWrite | HKPermissionNotify, "Hue", HKFormatFloat, HKUnitArcdegrees, new float(0), new float(360), new float(1));
    hueChar->setSetter(std::bind(&LEDAccessory::setHue, accessory, this, std::placeholders::_1));
    hueChar->setGetter(std::bind(&LEDAccessory::getHue, accessory, this));
    addCharacteristic(hueChar);
}

void LEDMode::addSaturationCharacteristic() {
    auto saturationChar = new HKCharacteristic(HKCharacteristicSaturation, HKValue(HKFormatFloat, 0), HKPermissionPairedRead | HKPermissionPairedWrite | HKPermissionNotify, "Saturation", HKFormatFloat, HKUnitPercentage, new float(0), new float(100), new float(1));
    saturationChar->setSetter(std::bind(&LEDAccessory::setSaturation, accessory, this, std::placeholders::_1));
    saturationChar->setGetter(std::bind(&LEDAccessory::getSaturation, accessory, this));
    addCharacteristic(saturationChar);
}

void LEDMode::setBrightness(uint8_t brightness, bool update) {
    #ifdef ALEXA_SUPPORT
    getCharacteristic(HKCharacteristicBrightness)->notify(HKValue(HKFormatInt, brightness));
    #endif
}

void LEDMode::setHue(float hue, bool update) {
    #ifdef ALEXA_SUPPORT
    getCharacteristic(HKCharacteristicHue)->notify(HKValue(HKFormatFloat, hue));
    #endif
}

void LEDMode::setSaturation(float saturation, bool update) {
    #ifdef ALEXA_SUPPORT
    getCharacteristic(HKCharacteristicSaturation)->notify(HKValue(HKFormatFloat, saturation));
    #endif
}
