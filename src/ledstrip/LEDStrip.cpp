/**
 * @file LEDStrip.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "ledstrip/LEDStrip.h"

LEDStrip::LEDStrip() : animator(std::make_shared<PixelAnimator>()) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        startColor[i] = HSIColor(0, 0, 0);
        currentColor[i] = HSIColor(0, 0, 0);
        endColor[i] = HSIColor(0, 0, 0);
    }
}

void LEDStrip::setPixelColor(uint16_t index, HSIColor color) {
    currentColor[index] = color;
}

HSIColor LEDStrip::getPixelColor(uint16_t index) {
    return currentColor[index];
}

void LEDStrip::clearTo(HSIColor color) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        currentColor[i] = color;
    }
}

void LEDStrip::setEndColorPixel(uint16_t index, HSIColor color) {
    if (animator->isAnimating()) {
        animator->stopAnimation();
    }
    endColor[index] = color;
}

void LEDStrip::clearEndColorTo(HSIColor color) {
    if (animator->isAnimating()) {
        animator->stopAnimation();
    }
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        endColor[i] = color;
    }
}

void LEDStrip::runAnimatorCallback(const AnimationParam &param) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        animatorCallback(i, startColor[i], endColor[i], param);
    }
}

void LEDStrip::startAnimation(uint16_t duration, LEDAnimatorUpdateCallback callback) {
    if (animator->isAnimating()) {
        animator->stopAnimation();
    }
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        startColor[i] = currentColor[i];
    }
    animatorCallback = callback;
    animator->startAnimation(duration, std::bind(&LEDStrip::runAnimatorCallback, this, std::placeholders::_1));
}

bool LEDStrip::animate() {
    if (!animator->isAnimating()) {
        return false;
    }
    animator->updateAnimation();
    return true;
}

bool LEDStrip::isAnimating() {
    return animator->isAnimating();
}
