/**
 * @file NeoPixelBusStrip.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef FAST_LED

#include "NeoPixelBusStrip.h"

NeoPixelBusStrip::NeoPixelBusStrip() : LEDStrip(), leds(NUM_LEDS) {
};

void NeoPixelBusStrip::setPixelColor(uint16_t index, HSIColor color) {
    LEDStrip::setPixelColor(index, color);

    RGBWColor converted = color.convertToRGBW();
    leds.SetPixelColor(index, RgbwColor(converted.red, converted.green, converted.blue, converted.white));
}

void NeoPixelBusStrip::clearTo(HSIColor color) {
    LEDStrip::clearTo(color);

    #ifdef RGBW_COLORS
    RGBWColor converted = color.convertToRGBW();
    leds.ClearTo(RgbwColor(converted.red, converted.green, converted.blue, converted.white));
    #else
    RGBColor converted = color.convertToRGB();
    leds.ClearTo(RgbColor(converted.red, converted.green, converted.blue));
    #endif
}

void NeoPixelBusStrip::setPixelColor(uint16_t index, RGBColor color) {
    leds.SetPixelColor(index, RgbColor(color.red, color.green, color.blue));
}

void NeoPixelBusStrip::clearTo(RGBColor color) {
    leds.ClearTo(RgbwColor(color.red, color.green, color.blue));
}

#ifdef RGBW_COLORS
void NeoPixelBusStrip::setPixelColor(uint16_t index, RGBWColor color) {
    leds.SetPixelColor(index, RgbwColor(color.red, color.green, color.blue, color.white));
}

void NeoPixelBusStrip::clearTo(RGBWColor color) {
    leds.ClearTo(RgbwColor(color.red, color.green, color.blue, color.white));
}
#endif

void NeoPixelBusStrip::show() {
    leds.Show();
}

void NeoPixelBusStrip::begin() {
    leds.Begin();
}

#endif
