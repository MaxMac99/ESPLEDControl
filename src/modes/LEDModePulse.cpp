//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_PULSE

#include "modes/LEDModePulse.h"

LEDModePulse::LEDModePulse(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Pulse", primary), brightness(100), hue(0), saturation(0), currentTarget(0, 0, 100), pulseStep(0), isRunning(false) {
}

void LEDModePulse::setup() {
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModePulse::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    currentTarget = HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress);
    if (!isRunning) {
        LEDHomeKit::shared()->getStrip()->setPixelColor(index, currentTarget);
        if (!LEDHomeKit::shared()->getStrip()->isAnimating() && index == NUM_LEDS - 1) {
            currentTarget.intensity = brightness;
            isRunning = true;
        }
    }
}

void LEDModePulse::start(bool cleanStart) {
    HKLOGINFO("Starting Pulse\r\n");
    pulseStep = 0;
    isRunning = cleanStart;
    if (!cleanStart) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

void LEDModePulse::update() {
    if (isRunning) {
        pulseStep += PULSE_STEP_SIZE;
        LEDHomeKit::shared()->getStrip()->clearTo(HSIColor(currentTarget.hue, currentTarget.saturation, (uint8_t) currentTarget.intensity * float(curveCubicwave8(pulseStep) / 255.0)));
        LEDHomeKit::shared()->getStrip()->show();
    }
}

void LEDModePulse::stop() {
    HKLOGINFO("Stopping Pulse\r\n");
    isRunning = false;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModePulse::getUpdateInterval() const {
    return PULSE_UPDATE_INTERVAL;
}

uint8_t LEDModePulse::getBrightness() {
    return brightness;
}

void LEDModePulse::setBrightness(uint8_t brightness, bool update) {
    LEDMode::setBrightness(brightness, update);
    LEDModePulse::brightness = brightness;
    if (update) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

float LEDModePulse::getHue() {
    return hue;
}

void LEDModePulse::setHue(float hue, bool update) {
    LEDMode::setHue(hue, update);
    LEDModePulse::hue = hue;
    if (update) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

float LEDModePulse::getSaturation() {
    return saturation;
}

void LEDModePulse::setSaturation(float saturation, bool update) {
    LEDMode::setSaturation(saturation, update);
    LEDModePulse::saturation = saturation;
    if (udpate) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

#endif
