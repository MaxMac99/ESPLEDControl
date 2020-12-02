//
// Created by Max Vissing on 2019-06-22.
//

#ifndef LED_HAP_ESP8266_LEDACCESSORY_H
#define LED_HAP_ESP8266_LEDACCESSORY_H

#include <HKAccessory.h>
#include "LEDMode.h"

#define LEDACCESSORY_CLASS_ID HKACCESSORY_CLASS_ID + 1

class LEDMode;

class LEDAccessory : public HKAccessory {
public:
    LEDAccessory(const String &accessoryName, const String &modelName, const String &firmwareRevision);
    virtual uint getClassId() { return LEDACCESSORY_CLASS_ID; };

    void setup() override;
    void run() override;

    LEDMode *getModeForId(uint32_t id);
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
    LEDMode *currentMode;
};


#endif //LED_HAP_ESP8266_LEDACCESSORY_H
