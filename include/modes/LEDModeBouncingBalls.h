//
// Created by Max Vissing on 2019-06-24.
//

#ifndef LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H
#define LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H

#include "../LEDMode.h"

#define UPDATE_INTERVAL 10
#define GRAVITY -9.81
#define START_HEIGHT 1
#define IMPACT_VELOCITY_START 4.42944691807  // sqrt(-2 * GRAVITY * START_HEIGHT)
#define NUM_BALLS 3

class LEDModeBouncingBalls : public LEDMode {
public:
    explicit LEDModeBouncingBalls(std::shared_ptr<CRGBSet> leds, LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start() override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void bounceBalls();
    CRGB randomColor();
private:
    uint8_t brightness;
    CRGB balls[NUM_BALLS];
    float height[NUM_BALLS];
    float impactVelocity[NUM_BALLS];
    float timeSinceLastBounce[NUM_BALLS];
    int   position[NUM_BALLS];
    long  clockTimeSinceLastBounce[NUM_BALLS];
    float dampening[NUM_BALLS];
};


#endif //LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H
