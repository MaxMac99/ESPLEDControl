//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_STATIC

#ifndef LED_HAP_ESP8266_LEDMODESTATIC_H
#define LED_HAP_ESP8266_LEDMODESTATIC_H

#include "../LEDMode.h"

class LEDMode;

class LEDModeStatic : public LEDMode {
public:
    explicit LEDModeStatic(LEDAccessory *accessory, bool primary=false);
    virtual void setup() override;
    virtual void start() override;
    virtual void update() override;
    virtual void stop() override;
    virtual uint8_t getBrightness() override;
    virtual void setBrightness(uint8_t brightness) override;
    virtual float getHue() override;
    virtual void setHue(float hue) override;
    virtual float getSaturation() override;
    virtual void setSaturation(float saturation) override;
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    uint8_t brightness;
    float saturation;
    float hue;
};


#endif //LED_HAP_ESP8266_LEDMODESTATIC_H

#endif
