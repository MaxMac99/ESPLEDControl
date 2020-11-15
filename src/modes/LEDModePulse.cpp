//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_PULSE

#include "modes/LEDModePulse.h"

LEDModePulse::LEDModePulse(std::shared_ptr<LEDStrip> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds), accessory, "Pulse", primary), brightness(100), hue(0), saturation(0), currentTarget(0, 0, 100), pulseStep(0), isRunning(false) {
}

void LEDModePulse::setup() {
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModePulse::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    currentTarget = HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress);
    if (!isRunning) {
        strip->setPixelColor(index, currentTarget);
        if (!strip->isAnimating() && index == NUM_LEDS - 1) {
            currentTarget.intensity = brightness;
            isRunning = true;
        }
    }
}

void LEDModePulse::start() {
    HKLOGINFO("Starting Pulse\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModePulse::update() {
    if (isRunning) {
        pulseStep += PULSE_STEP_SIZE;
        strip->clearTo(HSIColor(currentTarget.hue, currentTarget.saturation, (uint8_t) currentTarget.intensity * float(curveCubicwave8(pulseStep) / 255.0)));
        strip->show();
    }
}

void LEDModePulse::stop() {
    HKLOGINFO("Stopping Pulse\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModePulse::getUpdateInterval() const {
    return PULSE_UPDATE_INTERVAL;
}

uint8_t LEDModePulse::getBrightness() {
    return brightness;
}

void LEDModePulse::setBrightness(uint8_t brightness) {
    LEDModePulse::brightness = brightness;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModePulse::getHue() {
    return hue;
}

void LEDModePulse::setHue(float hue) {
    LEDModePulse::hue = hue;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModePulse::getSaturation() {
    return saturation;
}

void LEDModePulse::setSaturation(float saturation) {
    LEDModePulse::saturation = saturation;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModePulse::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
