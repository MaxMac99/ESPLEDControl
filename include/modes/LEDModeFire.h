//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_FIRE

#ifndef LED_HAP_ESP8266_LEDMODEFIRE_H
#define LED_HAP_ESP8266_LEDMODEFIRE_H

#include "../LEDMode.h"

#ifndef FIRE_UPDATE_INTERVAL
#define FIRE_UPDATE_INTERVAL 30
#endif

#ifndef FIRE_COOLING
#define FIRE_COOLING 45
#endif

#ifndef FIRE_SPARKING
#define FIRE_SPARKING 170
#endif

class LEDModeFire : public LEDMode {
public:
    explicit LEDModeFire(LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void fire();
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    uint8_t brightness;
    uint8_t currentBrightness;
    uint8_t startBrightness;
    byte heats[NUM_LEDS];
    bool isRunning;
};


#endif //LED_HAP_ESP8266_LEDMODEFIRE_H

#endif
