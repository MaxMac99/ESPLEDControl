//
// Created by Max Vissing on 2019-06-22.
//

#include "LEDAccessory.h"
#include "modes/AllLEDModes.h"

LEDAccessory::LEDAccessory(const String &accessoryName, const String &modelName, const String &firmwareRevision) : HKAccessory(accessoryName, modelName, firmwareRevision, HKAccessoryLightbulb), on(false), lastUpdate(0), currentMode(nullptr) {
}

void LEDAccessory::setup() {
    for (auto mode : generateModes(this)) {
        mode->setupCharacteristics();
        addService(mode);
    }
}

void LEDAccessory::run() {
    unsigned long now = millis();
    if (!on && now - lastUpdate > 10000) {
        LEDHomeKit::shared()->getStrip()->show();
        lastUpdate = now;
        return;
    }
    if (!currentMode) {
        if (LEDHomeKit::shared()->getStrip()->animate()) {
            LEDHomeKit::shared()->getStrip()->show();
        }
        return;
    }
    if (on && currentMode->getUpdateInterval() != 0 && now - lastUpdate > currentMode->getUpdateInterval()) {
        currentMode->update();
        lastUpdate = now;
    }
    if (LEDHomeKit::shared()->getStrip()->animate()) {
        LEDHomeKit::shared()->getStrip()->show();
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
    LEDHomeKit::shared()->getStrip()->show();
}

HKValue LEDAccessory::getOn(LEDMode *mode) {
    if (mode != currentMode) {
        return HKValue(HKFormatBool, false);
    }
    return HKValue(HKFormatBool, on);
}

void LEDAccessory::setBrightness(LEDMode *mode, const HKValue &value) {
    if (value.intValue == 0) {
        setOn(nullptr, HKValue(HKFormatBool, false));
        return;
    } else if (!on) {
        setOn(mode, HKValue(HKFormatBool, true));
    }
    if (on) {
        mode->setBrightness(value.intValue);
    }
}

HKValue LEDAccessory::getBrightness(LEDMode *mode) {
    return HKValue(HKFormatInt, mode->getBrightness());
}

void LEDAccessory::setHue(LEDMode *mode, const HKValue &value) {
    mode->setHue(value.floatValue);
}

HKValue LEDAccessory::getHue(LEDMode *mode) {
    return HKValue(HKFormatFloat, mode->getHue());
}

void LEDAccessory::setSaturation(LEDMode *mode, const HKValue &value) {
    mode->setSaturation(value.floatValue);
}

HKValue LEDAccessory::getSaturation(LEDMode *mode) {
    return HKValue(HKFormatFloat, mode->getSaturation());
}
