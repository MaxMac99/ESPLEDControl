//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_RAINBOW_CYCLE

#include "modes/LEDModeRainbowCycle.h"

LEDModeRainbowCycle::LEDModeRainbowCycle(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Rainbow", primary), brightness(100), currentBrightness(100), hue(0), hueAnimationEnabled(false) {

}

void LEDModeRainbowCycle::setup() {
    addBrightnessCharacteristic();
}

uint16_t LEDModeRainbowCycle::getWheelColor(uint16_t index, uint16_t hue) {
    return float(int(float(hue) + int(float(index) * float(RAINBOW_CYCLE_HUE_DELTA)) % 360) % 360);
}

void LEDModeRainbowCycle::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (hueAnimationEnabled) {
        HSIColor color = HSIColor::linearBlend<HueBlendClockwiseDirection>(startColor, endColor, param.progress);
        currentBrightness = color.intensity;
        LEDHomeKit::shared()->getStrip()->setPixelColor(index, color);
        if (!LEDHomeKit::shared()->getStrip()->isAnimating() && index == NUM_LEDS - 1) {
            hueAnimationEnabled = false;
        }
    } else {
        currentBrightness = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
    }
}

void LEDModeRainbowCycle::start(bool cleanStart) {
    HKLOGINFO("Starting Rainbow Cycle\r\n");
    if (cleanStart) {
        currentBrightness = 0;
        LEDHomeKit::shared()->getStrip()->clearTo(HSIColor(0, 100, 0));
    } else {
        hue = LEDHomeKit::shared()->getStrip()->getPixelColor(0).hue;
        for (int i = 0; i < NUM_LEDS; i++) {
            LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, HSIColor(getWheelColor(i, hue), 100, brightness));
        }
    }
    hueAnimationEnabled = !cleanStart;

    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
        LEDHomeKit::shared()->getStrip()->setPixelColor(i, color);
    }
    LEDHomeKit::shared()->getStrip()->show();
}

void LEDModeRainbowCycle::stop() {
    HKLOGINFO("Stopping Rainbow Cycle\r\n");
    hueAnimationEnabled = true;
    hue = (hue+1) % 360;
    for (int i = 0; i < NUM_LEDS; i++) {
        HSIColor color(getWheelColor(i, hue), 100, 0);
        LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, color);
    }
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModeRainbowCycle::getUpdateInterval() const {
    return RAINBOW_CYCLE_UPDATE_INTERVAL;
}

uint8_t LEDModeRainbowCycle::getBrightness() {
    return brightness;
}

void LEDModeRainbowCycle::setBrightness(uint8_t brightness) {
    LEDMode::setBrightness(brightness);
    HKLOGDEBUG("[LEDModeRainbowCycle::setBrightness] set brightness to %u\r\n", brightness);
    LEDModeRainbowCycle::brightness = brightness;
    for (int i = 0; i < NUM_LEDS; i++) {
        LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, HSIColor(getWheelColor(i, hue), 100, brightness));
    }
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRainbowCycle::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
