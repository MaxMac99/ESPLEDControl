//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_RAINBOW_CYCLE

#include "modes/LEDModeRainbowCycle.h"

LEDModeRainbowCycle::LEDModeRainbowCycle(std::shared_ptr<LEDStrip> strip, LEDAccessory *accessory, bool primary) : LEDMode(std::move(strip), accessory, primary), brightness(100), currentBrightness(100), hue(0), hueAnimationEnabled(false) {

}

void LEDModeRainbowCycle::setup() {
    addNameCharacteristic("Rainbow");
    addBrightnessCharacteristic();
}

uint16_t LEDModeRainbowCycle::getWheelColor(uint16_t index, uint16_t hue) {
    return float((hue + (index * RAINBOW_CYCLE_HUE_DELTA) % 360) % 360);
}

void LEDModeRainbowCycle::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (hueAnimationEnabled) {
        HSIColor color = HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress);
        currentBrightness = color.intensity;
        strip->setPixelColor(index, color);
        if (!strip->isAnimating() && index == NUM_LEDS - 1) {
            hueAnimationEnabled = false;
        }
    } else {
        currentBrightness = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
    }
}

void LEDModeRainbowCycle::start() {
    HKLOGINFO("Starting Rainbow Cycle\r\n");
    for (int i = 0; i < NUM_LEDS; i++) {
        strip->setEndColorPixel(i, HSIColor(getWheelColor(i, hue), 100, brightness));
    }
    hueAnimationEnabled = true;
    strip->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeRainbowCycle::update() {
    if (hueAnimationEnabled) {
        return;
    }
    hue += RAINBOW_CYCLE_STEP_SIZE;
    if (hue >= 360) {
        hue %= 360;
    }
    for (int i = 0; i < NUM_LEDS; i++) {
        HSIColor color(getWheelColor(i, hue), 100, currentBrightness);
        strip->setPixelColor(i, color);
    }
    strip->show();
}

void LEDModeRainbowCycle::stop() {
    HKLOGINFO("Stopping Rainbow Cycle\r\n");
    hueAnimationEnabled = true;
    strip->clearEndColorTo(HSIColor(hue, 100, 0));
    strip->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModeRainbowCycle::getUpdateInterval() const {
    return RAINBOW_CYCLE_UPDATE_INTERVAL;
}

uint8_t LEDModeRainbowCycle::getBrightness() {
    return brightness;
}

void LEDModeRainbowCycle::setBrightness(uint8_t brightness) {
    LEDModeRainbowCycle::brightness = brightness;
    for (int i = 0; i < NUM_LEDS; i++) {
        strip->setEndColorPixel(i, HSIColor(getWheelColor(i, hue), 100, brightness));
    }
    strip->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
