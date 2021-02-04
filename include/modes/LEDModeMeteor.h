//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_METEOR

#ifndef LED_HAP_ESP8266_LEDMODE_METEOR_H
#define LED_HAP_ESP8266_LEDMODE_METEOR_H

#include "../LEDMode.h"

#ifndef METEOR_UPDATE_INTERVAL
#define METEOR_UPDATE_INTERVAL 25
#endif

#ifndef METEOR_SIZE
#define METEOR_SIZE 5
#endif

#ifndef METEOR_TRAIL
#define METEOR_TRAIL 7
#endif

class LEDModeMeteor : public LEDMode {
public:
    explicit LEDModeMeteor(LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start(bool cleanStart) override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness, bool update) override;
    void meteor();
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    uint8_t brightness;
    uint8_t currentBrightness;
    uint8_t startBrightness;
    uint16_t headPos;
    bool isRunning;
};


#endif //LED_HAP_ESP8266_LEDMODEFIRE_H

#endif
