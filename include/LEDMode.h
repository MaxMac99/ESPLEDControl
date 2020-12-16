//
// Created by Max Vissing on 2019-04-18.
//

#ifndef LED_LIGHTS_LEDMODE_H
#define LED_LIGHTS_LEDMODE_H

#include <Arduino.h>
#include "LEDAccessory.h"
#include "ledstrip/LEDStrip.h"
#include "LEDHomeKit.h"

#define LEDMODE_CLASS_ID HKSERVICE_CLASS_ID + 1

class LEDAccessory;

enum Characteristic : uint8_t {
    OnOff,
    Brightness,
    Color,
    BrightnessColor
};

class LEDMode : public HKService {
public:
    explicit LEDMode(LEDAccessory *accessory, String name, bool primary=false);
    virtual ~LEDMode() = default;
    virtual uint getClassId() { return LEDMODE_CLASS_ID; };

    virtual void setup() = 0;
    virtual void start(bool cleanStart) = 0;
    virtual void update() = 0;
    virtual void stop() = 0;
    virtual unsigned long getUpdateInterval() const { return 0; };
    virtual uint8_t getBrightness() { return 0; }; // 0 - 100
    virtual void setBrightness(uint8_t brightness); // 0 - 100
    virtual float getHue() { return 0; }; // 0 - 360
    virtual void setHue(float hue); // 0 - 360
    virtual float getSaturation() { return 0; }; // 0 - 100
    virtual void setSaturation(float saturation); // 0 - 100
protected:
    void addNameCharacteristic(const char *name);
    void addBrightnessCharacteristic();
    void addHueCharacteristic();
    void addSaturationCharacteristic();
private:
    void setupCharacteristics();
    void turnOn(bool cleanStart);
    void turnOff();
    friend LEDAccessory;
protected:
    LEDAccessory *accessory;
};

#endif //LED_LIGHTS_LEDMODE_H
