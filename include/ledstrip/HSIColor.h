/**
 * @file HSIColor.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LED_LIGHTS_HSICOLOR_H
#define LED_LIGHTS_HSICOLOR_H

#include <Arduino.h>

struct RGBColor {
    RGBColor(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {};
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct RGBWColor {
    RGBWColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) : red(red), green(green), blue(blue), white(white) {};
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
};

struct HSIColor { 
    HSIColor() : hue(0), saturation(0), intensity(0) {};
    HSIColor(uint16_t hue, uint8_t saturation, uint8_t intensity) : hue(hue), saturation(saturation), intensity(intensity) {};
    uint16_t hue;
    uint8_t saturation;
    uint8_t intensity;
    
    RGBColor convertToRGB();
    RGBWColor convertToRGBW();

    template <typename T_HUEBLEND> static HSIColor linearBlend(const HSIColor &left, const HSIColor& right, float progress) {
        return HSIColor(
            int(T_HUEBLEND::HueBlend(float(left.hue) / 360, float(right.hue) / 360, progress) * 360),
            left.saturation + ((right.saturation - left.saturation) * progress),
            left.intensity + ((right.intensity - left.intensity) * progress)
        );
    };
};

#endif
