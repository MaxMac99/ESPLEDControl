/**
 * @file LEDModeRainbowStatic.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef MODE_FADE

#ifndef LED_HAP_ESP8266_LEDMODEFADE_H
#define LED_HAP_ESP8266_LEDMODEFADE_H

#include "../LEDMode.h"

#ifndef FADE_UPDATE_INTERVAL
#define FADE_UPDATE_INTERVAL 35
#endif

#ifndef FADE_STEP_SIZE
#define FADE_STEP_SIZE 1
#endif

class LEDModeFade : public LEDMode {
public:
    explicit LEDModeFade(LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    float brightness;
    float currentBrightness;
    uint16_t hue;
    bool hueAnimationEnabled;
};

#endif //LED_HAP_ESP8266_LEDMODEFADE_H

#endif
