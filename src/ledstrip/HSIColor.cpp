/**
 * @file HSIColor.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "ledstrip/HSIColor.h"

RGBColor HSIColor::convertToRGB() {
    uint8_t r, g, b;
    float hue_conv = PI * float(hue) / 180;
    float sat_conv = float(saturation) / 100;
    float int_conv = float(intensity) / 100;
    if (hue_conv < 2.09439) {
        r = 255 * int_conv / 3 * (1 + sat_conv * cos(hue_conv) / cos(1.047196667 - hue_conv)) + 0.5;
        g = 255 * int_conv / 3 * (1 + sat_conv * (1 - cos(hue_conv) / cos(1.047196667 - hue_conv))) + 0.5;
        b = 255 * int_conv / 3 * (1 - sat_conv) + 0.5;
    } else if (hue_conv < 4.188787) {
        hue_conv -= 2.09439;
        g = 255 * int_conv / 3 * (1 + sat_conv * cos(hue_conv) / cos(1.047196667 - hue_conv)) + 0.5;
        b = 255 * int_conv / 3 * (1 + sat_conv * (1 - cos(hue_conv) / cos(1.047196667 - hue_conv))) + 0.5;
        r = 255 * int_conv / 3 * (1 - sat_conv) + 0.5;
    } else {
        hue_conv -= 4.188787;
        b = 255 * int_conv / 3 * (1 + sat_conv * cos(hue_conv) / cos(1.047196667 - hue_conv)) + 0.5;
        r = 255 * int_conv / 3 * (1 + sat_conv * (1 - cos(hue_conv) / cos(1.047196667 - hue_conv))) + 0.5;
        g = 255 * int_conv / 3 * (1 - sat_conv) + 0.5;
    }

    return RGBColor(r, g, b);
}

RGBWColor HSIColor::convertToRGBW() {
    uint8_t r, g, b, w;
    float cos_h, cos_1047_h;
    float hue_conv = PI * float(hue) / 180;
    float sat_conv = float(saturation) / 100;
    float int_conv = float(intensity) / 100;
    if (hue_conv < 2.09439) {
        cos_h = cos(hue_conv);
        cos_1047_h = cos(1.047196667 - hue_conv);
        r = sat_conv * 255 * int_conv / 3 * (1 + cos_h / cos_1047_h);
        g = sat_conv * 255 * int_conv / 3 * (1 + (1 - cos_h / cos_1047_h));
        b = 0;
        w = 255 * (1 - sat_conv) * int_conv;
    } else if (hue_conv < 4.188787) {
        hue_conv -= 2.09439;
        cos_h = cos(hue_conv);
        cos_1047_h = cos(1.047196667 - hue_conv);
        g = sat_conv * 255 * int_conv / 3 * (1 + cos_h / cos_1047_h);
        b = sat_conv * 255 * int_conv / 3 * (1 + (1 - cos_h / cos_1047_h));
        r = 0;
        w = 255 * (1 - sat_conv) * int_conv;
    } else {
        hue_conv -= 4.188787;
        cos_h = cos(hue_conv);
        cos_1047_h = cos(1.047196667 - hue_conv);
        b = sat_conv * 255 * int_conv / 3 * (1 + cos_h / cos_1047_h);
        r = sat_conv * 255 * int_conv / 3 * (1 + (1 - cos_h / cos_1047_h));
        g = 0;
        w = 255 * (1 - sat_conv) * int_conv;
    }

    return RGBWColor(r, g, b, w);
}
