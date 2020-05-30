//
// Created by Max Vissing on 2019-06-22.
//

#ifndef LED_HAP_ESP8266_LEDACCESSORY_H
#define LED_HAP_ESP8266_LEDACCESSORY_H

#include <HKAccessory.h>
#include "LEDMode.h"
#include "FastLEDStrip.h"

class LEDMode;

class LEDAccessory : public HKAccessory {
public:
    LEDAccessory();

    void setup() override;
    void run() override;

    void setOn(LEDMode *mode, const HKValue &value);
    HKValue getOn(LEDMode *mode);
    void setBrightness(LEDMode *mode, const HKValue &value);
    HKValue getBrightness(LEDMode *mode);
    void setHue(LEDMode *mode, const HKValue &value);
    HKValue getHue(LEDMode *mode);
    void setSaturation(LEDMode *mode, const HKValue &value);
    HKValue getSaturation(LEDMode *mode);
private:
    bool on;
    unsigned int lastUpdate;
    std::shared_ptr<LEDStrip> strip;
    LEDMode *currentMode;
};


#endif //LED_HAP_ESP8266_LEDACCESSORY_H
