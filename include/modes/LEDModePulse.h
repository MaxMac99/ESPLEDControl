//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_PULSE

#ifndef LED_HAP_ESP8266_LEDMODEPULSE_H
#define LED_HAP_ESP8266_LEDMODEPULSE_H

#include "../LEDMode.h"
#include "ledstrip/Curves.h"

#ifndef PULSE_UPDATE_INTERVAL
#define PULSE_UPDATE_INTERVAL 35
#endif

#ifndef PULSE_STEP_SIZE
#define PULSE_STEP_SIZE 2
#endif

class LEDModePulse : public LEDMode {
public:
    explicit LEDModePulse(LEDAccessory *accessory, bool primary=false);
    virtual void setup();
    virtual void start(bool cleanStart);
    virtual void update();
    virtual void stop();
    virtual unsigned long getUpdateInterval() const;
    virtual uint8_t getBrightness();
    virtual void setBrightness(uint8_t brightness, bool update);
    virtual float getHue();
    virtual void setHue(float hue, bool update);
    virtual float getSaturation();
    virtual void setSaturation(float saturation, bool update);
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    uint8_t brightness;
    uint16_t hue;
    uint8_t saturation;
    HSIColor currentTarget;
    uint8_t pulseStep;
    bool isRunning;
};


#endif //LED_HAP_ESP8266_LEDMODEPULSE_H

#endif
