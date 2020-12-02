/**
 * @file LEDStrip.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LED_LIGHTS_LEDSTRIP_H
#define LED_LIGHTS_LEDSTRIP_H

#include <ESPHomeKit.h>
#include "PixelAnimator.h"
#include "HSIColor.h"

#include <HKDebug.h>

#ifndef NUM_LEDS
#define NUM_LEDS 144
#endif

#ifndef LED_PIN
#define LED_PIN 3
#endif

typedef std::function<void(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param)> LEDAnimatorUpdateCallback;

class LEDStrip {
public:
    LEDStrip();
    virtual ~LEDStrip() = default;
    virtual void setPixelColor(uint16_t index, HSIColor color);
    virtual void setPixelColor(uint16_t index, RGBColor color) {};
    virtual void setPixelColor(uint16_t index, RGBWColor color) {};
    HSIColor getPixelColor(uint16_t index);
    virtual void clearTo(HSIColor color);
    virtual void clearTo(RGBColor color) {};
    virtual void clearTo(RGBWColor color) {};
    virtual void show() = 0;
    virtual void begin() = 0;
    void setEndColorPixel(uint16_t index, HSIColor color);
    void clearEndColorTo(HSIColor color);
    void startAnimation(uint16_t duration, LEDAnimatorUpdateCallback callback);
    bool animate();
    bool isAnimating();
private:
    void runAnimatorCallback(const AnimationParam &param);
private:
    HSIColor currentColor[NUM_LEDS];
    HSIColor startColor[NUM_LEDS];
    HSIColor endColor[NUM_LEDS];
    std::shared_ptr<PixelAnimator> animator;
    LEDAnimatorUpdateCallback animatorCallback;
};

#endif
