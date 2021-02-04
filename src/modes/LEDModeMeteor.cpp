//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_METEOR

#include "modes/LEDModeMeteor.h"

LEDModeMeteor::LEDModeMeteor(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Meteor", primary), brightness(100), currentBrightness(0), startBrightness(0), isRunning(false) {
}

void LEDModeMeteor::setup() {
    addBrightnessCharacteristic();
}

void LEDModeMeteor::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (isRunning) {
        currentBrightness = startBrightness + (brightness - startBrightness) * param.progress;
        HKLOGDEBUG("[Meteor::handleAnimation] current: %u, start: %u, end: %u\r\n", currentBrightness, startBrightness, brightness);
    } else {
        HSIColor color = LEDHomeKit::shared()->getStrip()->getPixelColor(index);
        color.intensity = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
        LEDHomeKit::shared()->getStrip()->setPixelColor(index, color);
        if (!LEDHomeKit::shared()->getStrip()->isAnimating() && index == NUM_LEDS - 1) {
            currentBrightness = brightness;
            isRunning = true;
        }
    }
}

void LEDModeMeteor::start(bool cleanStart) {
    HKLOGINFO("Starting Meteor\r\n");
    headPos = 0;
    isRunning = cleanStart;
    if (!cleanStart) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeMeteor::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

void LEDModeMeteor::update() {
    if (isRunning) {
        meteor();
    }
}

void LEDModeMeteor::stop() {
    HKLOGINFO("Stopping Meteor\r\n");
    isRunning = false;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
    LEDHomeKit::shared()->getStrip()->clearTo(RGBColor(0, 0, 0));
}

unsigned long LEDModeMeteor::getUpdateInterval() const {
    return METEOR_UPDATE_INTERVAL;
}

uint8_t LEDModeMeteor::getBrightness() {
    return brightness;
}

void LEDModeMeteor::setBrightness(uint8_t brightness, bool update) {
    LEDMode::setBrightness(brightness, update);
    startBrightness = currentBrightness;
    LEDModeMeteor::brightness = brightness;
    HKLOGDEBUG("[Meteor::setBrightness] to: %u\r\n", brightness);
    if (update && isRunning) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeMeteor::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

void LEDModeMeteor::meteor() {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (random(10) > 5) {
            HSIColor old = LEDHomeKit::shared()->getStrip()->getPixelColor(i);
            if (METEOR_TRAIL > old.intensity) {
                old.intensity = 0;
            } else {
                old.intensity -= METEOR_TRAIL;
            }
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, old);
        }
    }
    for (int i = 0; i < METEOR_SIZE; i++) {
        if (i > headPos) {
            break;
        }
        if (headPos - i < NUM_LEDS) {
            LEDHomeKit::shared()->getStrip()->setPixelColor(headPos - i, HSIColor(240, 60, currentBrightness));
        } 
    }
    headPos++;
    if (headPos >= 2 * NUM_LEDS) {
        headPos = 0;
        LEDHomeKit::shared()->getStrip()->clearTo(HSIColor(0, 0, 0));
    }
    LEDHomeKit::shared()->getStrip()->show();
}

#endif
