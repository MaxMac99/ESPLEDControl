//
// Created by Max Vissing on 2019-06-23.
//

#ifdef MODE_RAINBOW_CYCLE

#ifndef LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H
#define LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H

#include "../LEDMode.h"

#ifndef RAINBOW_CYCLE_UPDATE_INTERVAL
#define RAINBOW_CYCLE_UPDATE_INTERVAL 35
#endif

#ifndef RAINBOW_CYCLE_HUE_DELTA
#define RAINBOW_CYCLE_HUE_DELTA 3
#endif

#ifndef RAINBOW_CYCLE_STEP_SIZE
#define RAINBOW_CYCLE_STEP_SIZE 1
#endif

class LEDModeRainbowCycle : public LEDMode {
public:
    explicit LEDModeRainbowCycle(LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
    uint16_t getWheelColor(uint16_t index, uint16_t hue);
private:
    uint8_t brightness;
    float currentBrightness;
    uint16_t hue;
    bool hueAnimationEnabled;
};


#endif //LED_HAP_ESP8266_LEDMODERAINBOWCYCLE_H

#endif
