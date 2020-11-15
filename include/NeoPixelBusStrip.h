/**
 * @file NeoPixelBusStrip.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef FAST_LED

#ifndef LED_LIGHTS_NEOPIXELBUSSTRIP_H
#define LED_LIGHTS_NEOPIXELBUSSTRIP_H

#include <NeoPixelBus.h>
#include "ledstrip/LEDStrip.h"

class NeoPixelBusStrip : public LEDStrip {
public:
    NeoPixelBusStrip();
    void setPixelColor(uint16_t index, HSIColor color);
    void clearTo(HSIColor color);
    void setPixelColor(uint16_t index, RGBColor color);
    void clearTo(RGBColor color);

    #ifdef RGBW_COLORS
    void setPixelColor(uint16_t index, RGBWColor color);
    void clearTo(RGBWColor color);
    #endif
    void show();
    void begin();
private:
    NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> leds;
};

#endif

#endif
