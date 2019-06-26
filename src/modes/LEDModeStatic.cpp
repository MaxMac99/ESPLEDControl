//
// Created by Max Vissing on 2019-06-23.
//

#include "modes/LEDModeStatic.h"

LEDModeStatic::LEDModeStatic(std::shared_ptr <CRGBSet> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds), accessory, primary), brightness(100), hue(0), saturation(0) {
}

void LEDModeStatic::setup() {
    addNameCharacteristic("Static");
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModeStatic::start() {
    HKLOGINFO("Starting Static\r\n");
    setColor();
}

void LEDModeStatic::update() {

}

void LEDModeStatic::stop() {
    HKLOGINFO("Stopping Static\r\n");
}

uint8_t LEDModeStatic::getBrightness() {
    return brightness;
}

void LEDModeStatic::setBrightness(uint8_t brightness) {
    LEDModeStatic::brightness = brightness;
    setColor();
}

float LEDModeStatic::getHue() {
    return hue;
}

void LEDModeStatic::setHue(float hue) {
    LEDModeStatic::hue = hue;
    setColor();
}

float LEDModeStatic::getSaturation() {
    return saturation;
}

void LEDModeStatic::setSaturation(float saturation) {
    LEDModeStatic::saturation = saturation;
    setColor();
}

void LEDModeStatic::setColor() {
    uint8_t hueConv = float(hue) / float(360.0 / 255.0) + 0.5;
    HKLOGINFO("[LEDModeStatic::setColor] hue=%f convHue=%d\r\n", hue, hueConv);
    uint8_t saturationConv = saturation * 2.55 + 0.5;
    HKLOGINFO("[LEDModeStatic::setColor] hue=%f convHue=%d\r\n", saturation, saturationConv);
    uint8_t brightnessConv = brightness * 2.55 + 0.5;
    HKLOGINFO("[LEDModeStatic::setColor] hue=%d convHue=%d\r\n", brightness, brightnessConv);
    leds->fill_solid(CHSV(hueConv, saturationConv, brightnessConv));
    FastLED.show();
}
