//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_BOUNCING_BALLS

#ifndef LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H
#define LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H

#include "../LEDMode.h"

#ifndef BOUNCING_BALLS_UPDATE_INTERVAL
#define BOUNCING_BALLS_UPDATE_INTERVAL 10
#endif

#ifndef BOUNCING_BALLS_GRAVITY
#define BOUNCING_BALLS_GRAVITY -9.81
#endif

#ifndef BOUNCING_BALLS_START_HEIGHT
#define BOUNCING_BALLS_START_HEIGHT 1
#endif

#ifndef BOUNCING_BALLS_IMPACT_VELOCITY_START
#define BOUNCING_BALLS_IMPACT_VELOCITY_START 4.42944691807  // sqrt(-2 * GRAVITY * START_HEIGHT)
#endif

#ifndef BOUNCING_BALLS_NUM_BALLS
#define BOUNCING_BALLS_NUM_BALLS 3
#endif

class LEDModeBouncingBalls : public LEDMode {
public:
    explicit LEDModeBouncingBalls(LEDAccessory *accessory, bool primary=false);
    void setup() override;
    void start(bool cleanStart) override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    float getHue();
    void setHue(float hue);
    float getSaturation();
    void setSaturation(float saturation);
    void bounceBalls();
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    uint8_t brightness;
    uint16_t hue;
    uint8_t saturation;
    HSIColor currentTarget;
    float height[BOUNCING_BALLS_NUM_BALLS];
    float impactVelocity[BOUNCING_BALLS_NUM_BALLS];
    float timeSinceLastBounce[BOUNCING_BALLS_NUM_BALLS];
    int   position[BOUNCING_BALLS_NUM_BALLS];
    long  clockTimeSinceLastBounce[BOUNCING_BALLS_NUM_BALLS];
    float dampening[BOUNCING_BALLS_NUM_BALLS];
    bool isRunning;
};


#endif //LED_HAP_ESP8266_LEDMODEBOUNCINGBALLS_H

#endif
