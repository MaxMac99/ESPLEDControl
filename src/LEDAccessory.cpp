//
// Created by Max Vissing on 2019-06-22.
//

#include "LEDAccessory.h"
#include "modes/AllLEDModes.h"

LEDAccessory::LEDAccessory() : HKAccessory(HKAccessoryLightbulb), on(false), lastUpdate(0), currentMode(nullptr) {
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(0);
}

void LEDAccessory::setup() {
    for (auto mode : generateModes(std::make_shared<CRGBSet>(leds), this)) {
        mode->setupCharacteristics();
        addService(mode);
    }
}

void LEDAccessory::run() {
    unsigned long now = millis();
    if (!on && now - lastUpdate > 10000) {
        FastLED.show();
        lastUpdate = now;
        return;
    }
    if (!currentMode) {
        return;
    }
    if (on && currentMode->getUpdateInterval() != 0 && now - lastUpdate > currentMode->getUpdateInterval()) {
        currentMode->update();
        lastUpdate = now;
    }
}

void LEDAccessory::setOn(LEDMode *mode, const HKValue& value) {
    HKLOGINFO("[LEDAccessory::setOn] mode: %s, value: %d\r\n", mode->getCharacteristic(HKCharacteristicName)->getValue().stringValue, value.boolValue);
    if (!currentMode) {
        currentMode = mode;
    }
    HKLOGINFO("[LEDAccessory::setOn] currentMode: %s\r\n", currentMode->getCharacteristic(HKCharacteristicName)->getValue().stringValue);
    HKLOGINFO("[LEDAccessory::setOn] on: %d\r\n", on);
    if (on == value.boolValue && currentMode == mode) {
        return;
    }
    if (value.boolValue && currentMode != mode && mode != nullptr) {
        currentMode->turnOff();
        currentMode = mode;
    }
    if (value.boolValue) {
        FastLED.setBrightness(MAX_BRIGHTNESS);
        currentMode->turnOn();
        on = true;
    } else if (currentMode == mode) {
        currentMode->turnOff();
        FastLED.setBrightness(0);
        on = false;
    }
    FastLED.show();
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

uint8_t LEDAccessory::convertBrightness(uint8_t brightness, uint8_t localMax) {
    return float(brightness) / float(localMax) * MAX_BRIGHTNESS;
}
