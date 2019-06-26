//
// Created by Max Vissing on 2019-06-23.
//

#include "modes/LEDModeRainbowCycle.h"

LEDModeRainbowCycle::LEDModeRainbowCycle(std::shared_ptr<CRGBSet> leds, LEDAccessory *accessory, bool primary)
        : LEDMode(std::move(leds), accessory, primary), brightness(100), hue(0) {

}

void LEDModeRainbowCycle::setup() {
    addNameCharacteristic("Rainbow");
    addBrightnessCharacteristic();
}

void LEDModeRainbowCycle::start() {
    HKLOGINFO("Starting Rainbow Cycle\r\n");
    setBrightness(brightness);
}

void LEDModeRainbowCycle::update() {
    hue += STEP_SIZE;
    leds->fill_rainbow(hue, HUE_DELTA);
    FastLED.show();
}

void LEDModeRainbowCycle::stop() {
    HKLOGINFO("Stopping Rainbow Cycle\r\n");
}

unsigned long LEDModeRainbowCycle::getUpdateInterval() const {
    return UPDATE_INTERVAL;
}

uint8_t LEDModeRainbowCycle::getBrightness() {
    return brightness;
}

void LEDModeRainbowCycle::setBrightness(uint8_t brightness) {
    LEDModeRainbowCycle::brightness = brightness;
    uint8_t brightnessConv = LEDAccessory::convertBrightness(brightness, 100);
    FastLED.setBrightness(brightnessConv);
}
