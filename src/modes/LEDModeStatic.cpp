//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_STATIC

#include "modes/LEDModeStatic.h"

LEDModeStatic::LEDModeStatic(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Static", primary), brightness(100), saturation(0), hue(0) {
}

void LEDModeStatic::setup() {
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModeStatic::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    LEDHomeKit::shared()->getStrip()->setPixelColor(index, HSIColor::linearBlend<HueBlendClockwiseDirection>(startColor, endColor, param.progress));
}

void LEDModeStatic::start(bool cleanStart) {
    HKLOGINFO("Starting Static\r\n");
    if (cleanStart) {
        LEDHomeKit::shared()->getStrip()->clearTo(HSIColor(hue, saturation, 0));
    }
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeStatic::update() {
}

void LEDModeStatic::stop() {
    HKLOGINFO("Stopping Static\r\n");
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, 0));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

uint8_t LEDModeStatic::getBrightness() {
    return brightness;
}

void LEDModeStatic::setBrightness(uint8_t brightness, bool update) {
    LEDMode::setBrightness(brightness, update);
    LEDModeStatic::brightness = brightness;
    HKLOGDEBUG("[Static] Set brightness to %d with update %d\r\n", brightness, update);
    if (update) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

float LEDModeStatic::getHue() {
    return hue;
}

void LEDModeStatic::setHue(float hue, bool update) {
    LEDMode::setHue(hue, update);
    LEDModeStatic::hue = hue;
    HKLOGDEBUG("[Static] Set hue to %f with update %d\r\n", hue, update);
    if (update) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

float LEDModeStatic::getSaturation() {
    return saturation;
}

void LEDModeStatic::setSaturation(float saturation, bool update) {
    LEDMode::setSaturation(saturation, update);
    LEDModeStatic::saturation = saturation;
    HKLOGDEBUG("[Static] Set saturation to %f with update %d\r\n", saturation, update);
    if (update) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

#endif
