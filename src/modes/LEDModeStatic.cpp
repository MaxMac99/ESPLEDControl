//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_STATIC

#include "modes/LEDModeStatic.h"

LEDModeStatic::LEDModeStatic(std::shared_ptr<LEDStrip> strip, LEDAccessory *accessory, bool primary) : LEDMode(std::move(strip), accessory, "Static", primary), brightness(100), saturation(0), hue(0) {
}

void LEDModeStatic::setup() {
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModeStatic::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    strip->setPixelColor(index, HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress));
}

void LEDModeStatic::start() {
    HKLOGINFO("Starting Static\r\n");
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeStatic::update() {
}

void LEDModeStatic::stop() {
    HKLOGINFO("Stopping Static\r\n");
    strip->clearEndColorTo(HSIColor(hue, saturation, 0));
    strip->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

uint8_t LEDModeStatic::getBrightness() {
    return brightness;
}

void LEDModeStatic::setBrightness(uint8_t brightness) {
    LEDModeStatic::brightness = brightness;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeStatic::getHue() {
    return hue;
}

void LEDModeStatic::setHue(float hue) {
    LEDModeStatic::hue = hue;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeStatic::getSaturation() {
    return saturation;
}

void LEDModeStatic::setSaturation(float saturation) {
    LEDModeStatic::saturation = saturation;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
