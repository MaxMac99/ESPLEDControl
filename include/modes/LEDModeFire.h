//
// Created by Max Vissing on 2019-06-24.
//

#ifndef LED_HAP_ESP8266_LEDMODEFIRE_H
#define LED_HAP_ESP8266_LEDMODEFIRE_H

#include "../LEDMode.h"

#define UPDATE_INTERVAL 30
#define COOLING 45
#define SPARKING 170

class LEDModeFire : public LEDMode {
public:
    explicit LEDModeFire(std::shared_ptr<CRGBSet> leds, LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void fire();
private:
    uint8_t brightness;
    byte heats[NUM_LEDS];
};


#endif //LED_HAP_ESP8266_LEDMODEFIRE_H
