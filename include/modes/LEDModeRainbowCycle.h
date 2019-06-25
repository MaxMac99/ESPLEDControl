//
// Created by Max Vissing on 2019-06-23.
//

#ifndef LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H
#define LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H

#include "../LEDMode.h"

#define UPDATE_INTERVAL 35
#define HUE_DELTA 2
#define STEP_SIZE 2

class LEDModeRainbowCycle : public LEDMode {
public:
    explicit LEDModeRainbowCycle(std::shared_ptr<CRGBSet> leds, LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
private:
    uint8_t brightness;
    uint8_t hue;
};


#endif //LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H
