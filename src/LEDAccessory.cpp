//
// Created by Max Vissing on 2019-06-22.
//

#include "LEDAccessory.h"
#include "modes/AllLEDModes.h"

LEDAccessory::LEDAccessory() : HKAccessory(HKAccessoryLightbulb), on(false), lastUpdate(0), strip(std::make_shared<FastLEDStrip>()), currentMode(nullptr) {
    strip->begin();
}

void LEDAccessory::setup() {
    for (auto mode : generateModes(strip, this)) {
        mode->setupCharacteristics();
        addService(mode);
    }
}

void LEDAccessory::run() {
    unsigned long now = millis();
    if (!on && now - lastUpdate > 10000) {
        strip->show();
        lastUpdate = now;
        return;
    }
    if (!currentMode) {
        if (strip->animate()) {
            strip->show();
        }
        return;
    }
    if (on && currentMode->getUpdateInterval() != 0 && now - lastUpdate > currentMode->getUpdateInterval()) {
        currentMode->update();
        lastUpdate = now;
    }
    if (strip->animate()) {
        strip->show();
    }
}

void LEDAccessory::setOn(LEDMode *mode, const HKValue& value) {
    if (!currentMode) {
        currentMode = mode;
    }
    if (on == value.boolValue && currentMode == mode) {
        return;
    }
    if (value.boolValue && currentMode != mode && mode != nullptr) {
        currentMode->turnOff();
        currentMode = mode;
    }
    if (value.boolValue) {
        currentMode->turnOn();
        on = true;
    } else if (currentMode == mode) {
        currentMode->turnOff();
        on = false;
    }
    strip->show();
}

HKValue LEDAccessory::getOn(LEDMode *mode) {
    if (mode != currentMode) {
        return HKValue(FormatBool, false);
    }
    return HKValue(FormatBool, on);
}

void LEDAccessory::setBrightness(LEDMode *mode, const HKValue &value) {
    if (value.intValue == 0) {
        setOn(nullptr, HKValue(FormatBool, false));
        return;
    }
    if (on) {
        mode->setBrightness(value.intValue);
    }
}

HKValue LEDAccessory::getBrightness(LEDMode *mode) {
    return HKValue(FormatInt, mode->getBrightness());
}

void LEDAccessory::setHue(LEDMode *mode, const HKValue &value) {
    mode->setHue(value.floatValue);
}

HKValue LEDAccessory::getHue(LEDMode *mode) {
    return HKValue(FormatFloat, mode->getHue());
}

void LEDAccessory::setSaturation(LEDMode *mode, const HKValue &value) {
    mode->setSaturation(value.floatValue);
}

HKValue LEDAccessory::getSaturation(LEDMode *mode) {
    return HKValue(FormatFloat, mode->getSaturation());
}
