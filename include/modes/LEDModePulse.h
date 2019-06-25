//
// Created by Max Vissing on 2019-06-24.
//

#ifndef LED_HAP_ESP8266_LEDMODEPULSE_H
#define LED_HAP_ESP8266_LEDMODEPULSE_H

#include "../LEDMode.h"

#define UPDATE_INTERVAL 35
#define STEP_SIZE 2

class LEDModePulse : public LEDMode {
public:
    explicit LEDModePulse(std::shared_ptr <CRGBSet> leds, LEDAccessory *accessory, bool primary=false);

    virtual void setup();

    virtual void start();

    virtual void update();

    virtual void stop();

    virtual unsigned long getUpdateInterval() const;

    virtual uint8_t getBrightness();

    virtual void setBrightness(uint8_t brightness);

    virtual float getHue();

    virtual void setHue(float hue);

    virtual float getSaturation();

    virtual void setSaturation(float saturation);

private:
    void setColor();
private:
    uint8_t brightness;
    uint8_t hue;
    uint8_t saturation;
    uint8_t pulseStep;
};


#endif //LED_HAP_ESP8266_LEDMODEPULSE_H
