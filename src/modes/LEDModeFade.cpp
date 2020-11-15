/**
 * @file LEDModeRainbowStatic.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef MODE_FADE

#include "modes/LEDModeFade.h"

LEDModeFade::LEDModeFade(std::shared_ptr<LEDStrip> strip, LEDAccessory *accessory, bool primary) : LEDMode(std::move(strip), accessory, "Fade", primary), brightness(100), currentBrightness(100), hue(0), hueAnimationEnabled(false) {

}

void LEDModeFade::setup() {
    addBrightnessCharacteristic();
}

void LEDModeFade::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (hueAnimationEnabled) {
        HSIColor color = HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress);
        currentBrightness = color.intensity;
        strip->setPixelColor(index, color);
        if (!strip->isAnimating() && index == NUM_LEDS - 1) {
            HKLOGINFO("[LEDModeFade::handleAnimation] end animation\r\n");
            hueAnimationEnabled = false;
        }
    } else {
        currentBrightness = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
    }
}

void LEDModeFade::start() {
    HKLOGINFO("Starting Fade\r\n");
    strip->clearEndColorTo(HSIColor(hue, 100, brightness));
    hueAnimationEnabled = true;
    strip->startAnimation(500, std::bind(&LEDModeFade::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeFade::update() {
    if (hueAnimationEnabled) {
        return;
    }
    hue += FADE_STEP_SIZE;
    if (hue >= 360) {
        hue %= 360;
    }
    
    strip->clearTo(HSIColor(hue, 100, currentBrightness));
    strip->show();
}

void LEDModeFade::stop() {
    HKLOGINFO("Stopping Fade\r\n");
    hueAnimationEnabled = true;
    strip->clearEndColorTo(HSIColor(hue, 100, 0));
    strip->startAnimation(500, std::bind(&LEDModeFade::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModeFade::getUpdateInterval() const {
    return FADE_UPDATE_INTERVAL;
}

uint8_t LEDModeFade::getBrightness() {
    return brightness;
}

void LEDModeFade::setBrightness(uint8_t brightness) {
    LEDModeFade::brightness = brightness;
    strip->clearEndColorTo(HSIColor(hue, 100, brightness));
    strip->startAnimation(500, std::bind(&LEDModeFade::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
