/**
 * @file FastLEDStrip.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "FastLEDStrip.h"

FastLEDStrip::FastLEDStrip() : LEDStrip() {
}

void FastLEDStrip::setPixelColor(uint16_t index, HSIColor color) {
    LEDStrip::setPixelColor(index, color);

    RGBColor converted = color.convertToRGB();
    leds[index].setRGB(converted.red, converted.green, converted.blue);
}

void FastLEDStrip::setPixelColor(uint16_t index, RGBColor color) {
    // HKLOGINFO("r: %d g: %d, b: %d\r\n", color.red, color.green, color.blue);
    leds[index].setRGB(color.red, color.green, color.blue);
}

void FastLEDStrip::clearTo(HSIColor color) {
    LEDStrip::clearTo(color);

    RGBColor converted = color.convertToRGB();
    leds.fill_solid(CRGB(converted.red, converted.green, converted.blue));
}

void FastLEDStrip::clearTo(RGBColor color) {
    leds.fill_solid(CRGB(color.red, color.green, color.blue));
}

void FastLEDStrip::show() {
    FastLED.show();
}

void FastLEDStrip::begin() {
    CFastLED::addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(MAX_BRIGHTNESS);
    // FastLED.setCorrection();
}
