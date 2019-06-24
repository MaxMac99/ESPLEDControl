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
    Serial.println("Setup");
    for (auto mode : generateModes(std::make_shared<CRGBSet>(leds), this)) {
        mode->setupCharacteristics();
        if (auto name = mode->getCharacteristic(HKCharacteristicName)) {
            Serial.println("addService " + String(name->getValue().stringValue));
        } else {
            Serial.println("addService unknown name");
        }
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
    Serial.println("Set LED On: " + String(value.boolValue));

    if (!currentMode) {
        currentMode = mode;
    }
    if (on == value.boolValue && currentMode == mode) {
        return;
    }
    if (on && currentMode != mode && mode != nullptr) {
        currentMode->turnOff();
        currentMode = mode;
    }
    on = value.boolValue;
    if (on) {
        FastLED.setBrightness(MAX_BRIGHTNESS);
        currentMode->turnOn();
    } else {
        currentMode->turnOff();
        FastLED.setBrightness(0);
    }
    FastLED.show();
}

HKValue LEDAccessory::getOn() {
    return HKValue(FormatBool, on);
}

void LEDAccessory::setBrightness(LEDMode *mode, const HKValue &value) {
    Serial.println("Set LED Brightness: " + String(value.intValue));

    if (value.intValue == 0) {
        setOn(nullptr, HKValue(FormatBool, false));
        return;
    }
    if (on) {
        mode->setBrightness(value.intValue);
    }
}

HKValue LEDAccessory::getBrightness() {
    if (!currentMode) {
        return HKValue(FormatInt, 0);
    }
    return HKValue(FormatInt, currentMode->getBrightness());
}

void LEDAccessory::setHue(LEDMode *mode, const HKValue &value) {
    Serial.println("Set LED Hue: " + String(value.floatValue));

    mode->setHue(value.floatValue);
}

HKValue LEDAccessory::getHue() {
    if (!currentMode) {
        return HKValue(FormatFloat, 0);
    }
    return HKValue(FormatFloat, currentMode->getHue());
}

void LEDAccessory::setSaturation(LEDMode *mode, const HKValue &value) {
    Serial.println("Set LED Saturation: " + String(value.floatValue));
    mode->setSaturation(value.floatValue);
}

HKValue LEDAccessory::getSaturation() {
    if (!currentMode) {
        return HKValue(FormatFloat, 0);
    }
    return HKValue(FormatFloat, currentMode->getSaturation());
}

uint8_t LEDAccessory::convertBrightness(uint8_t brightness, uint8_t localMax) {
    return float(brightness) / float(localMax) * MAX_BRIGHTNESS;
}
