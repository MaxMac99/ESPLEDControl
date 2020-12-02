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
    LEDHomeKit::shared()->getStrip()->setPixelColor(index, HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress));
}

void LEDModeStatic::start() {
    HKLOGINFO("Starting Static\r\n");
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

void LEDModeStatic::setBrightness(uint8_t brightness) {
    LEDMode::setBrightness(brightness);
    LEDModeStatic::brightness = brightness;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeStatic::getHue() {
    return hue;
}

void LEDModeStatic::setHue(float hue) {
    LEDMode::setHue(hue);
    LEDModeStatic::hue = hue;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeStatic::getSaturation() {
    return saturation;
}

void LEDModeStatic::setSaturation(float saturation) {
    LEDMode::setSaturation(saturation);
    LEDModeStatic::saturation = saturation;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(hue, saturation, brightness));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeStatic::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

#endif
