//
// Created by Max Vissing on 2019-04-18.
//

#ifndef LED_LIGHTS_LEDMODE_H
#define LED_LIGHTS_LEDMODE_H

#include <Arduino.h>
#include "LEDAccessory.h"
#include "ledstrip/LEDStrip.h"

class LEDAccessory;

enum Characteristic : uint8_t {
    OnOff,
    Brightness,
    Color,
    BrightnessColor
};

class LEDAccessory;

class LEDMode : public HKService {
public:
    explicit LEDMode(std::shared_ptr<LEDStrip> strip, LEDAccessory *accessory, bool primary=false);
    virtual ~LEDMode() = default;
    virtual void setup() = 0;
    virtual void start() = 0;
    virtual void update() = 0;
    virtual void stop() = 0;
    virtual unsigned long getUpdateInterval() const { return 0; };
    virtual uint8_t getBrightness() { return 0; }; // 0 - 100
    virtual void setBrightness(uint8_t brightness) {}; // 0 - 100
    virtual float getHue() { return 0; }; // 0 - 360
    virtual void setHue(float hue) {}; // 0 - 360
    virtual float getSaturation() { return 0; }; // 0 - 100
    virtual void setSaturation(float saturation) {}; // 0 - 100
protected:
    void addNameCharacteristic(const char *name);
    void addBrightnessCharacteristic();
    void addHueCharacteristic();
    void addSaturationCharacteristic();
private:
    void setupCharacteristics();
    void turnOn();
    void turnOff();
    friend LEDAccessory;
protected:
    LEDAccessory *accessory;
    std::shared_ptr<LEDStrip> strip;
};

#endif //LED_LIGHTS_LEDMODE_H
