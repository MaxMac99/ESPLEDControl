//
// Created by Max Vissing on 2019-06-22.
//

#ifndef LED_HAP_ESP8266_LEDACCESSORY_H
#define LED_HAP_ESP8266_LEDACCESSORY_H

#include <FastLED.h>
#include "HKAccessory.h"
#include "LEDMode.h"

#define LED_PIN 4
#define NUM_LEDS 144
#define MAX_BRIGHTNESS 200 // Max 255

class LEDMode;

class LEDAccessory : public HKAccessory {
public:
    LEDAccessory();

    void setup() override;
    void run() override;

    void setOn(LEDMode *mode, const HKValue &value);
    HKValue getOn();
    void setBrightness(LEDMode *mode, const HKValue &value);
    HKValue getBrightness();
    void setHue(LEDMode *mode, const HKValue &value);
    HKValue getHue();
    void setSaturation(LEDMode *mode, const HKValue &value);
    HKValue getSaturation();
    static uint8_t convertBrightness(uint8_t brightness, uint8_t localMax);
private:
    bool on;
    unsigned int lastUpdate;
    CRGBArray<NUM_LEDS> leds;
    LEDMode *currentMode;
};


#endif //LED_HAP_ESP8266_LEDACCESSORY_H
