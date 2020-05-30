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

#ifndef LED_LIGHTS_FASTLEDSTRIP_H
#define LED_LIGHTS_FASTLEDSTRIP_H

#ifndef MAX_BRIGHTNESS
#define MAX_BRIGHTNESS 255 // Max 255
#endif

#include <FastLED.h>
#include "ledstrip/LEDStrip.h"

class FastLEDStrip : public LEDStrip {
public:
    FastLEDStrip();
    void setPixelColor(uint16_t index, HSIColor color);
    void setPixelColor(uint16_t index, RGBColor color);
    void clearTo(HSIColor color);
    void clearTo(RGBColor color);
    void show();
    void begin();
private:
    CRGBArray<NUM_LEDS> leds;
};

#endif
