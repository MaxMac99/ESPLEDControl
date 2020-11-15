//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_BOUNCING_BALLS

#include "modes/LEDModeBouncingBalls.h"

LEDModeBouncingBalls::LEDModeBouncingBalls(std::shared_ptr<LEDStrip> leds, LEDAccessory *accessory, "Balls", bool primary) : LEDMode(std::move(leds), accessory, primary), brightness(100), hue(0), saturation(0), currentTarget(0, 0, 100), height(), impactVelocity(), timeSinceLastBounce(), position(), clockTimeSinceLastBounce(), dampening(), isRunning(false) {
    for (int i = 0 ; i < BOUNCING_BALLS_NUM_BALLS; i++) {
        clockTimeSinceLastBounce[i] = millis();
        height[i] = BOUNCING_BALLS_START_HEIGHT;
        position[i] = 0;
        impactVelocity[i] = BOUNCING_BALLS_IMPACT_VELOCITY_START;
        timeSinceLastBounce[i] = 0;
        dampening[i] = 0.90 - float(i) / pow(BOUNCING_BALLS_NUM_BALLS, 2);
    }
}

void LEDModeBouncingBalls::setup() {
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModeBouncingBalls::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    currentTarget = HSIColor::linearBlend<HueBlendShortestDistance>(startColor, endColor, param.progress);
    if (!isRunning) {
        strip->setPixelColor(index, currentTarget);
        if (!strip->isAnimating() && index == NUM_LEDS - 1) {
            currentTarget.intensity = brightness;
            isRunning = true;
        }
    }
}

void LEDModeBouncingBalls::start() {
    HKLOGINFO("Starting Bouncing Balls\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->startAnimation(500, std::bind(&LEDModeBouncingBalls::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeBouncingBalls::update() {
    if (isRunning) {
        bounceBalls();
    }
}

void LEDModeBouncingBalls::stop() {
    HKLOGINFO("Stopping Bouncing Balls\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->startAnimation(500, std::bind(&LEDModeBouncingBalls::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModeBouncingBalls::getUpdateInterval() const {
    return BOUNCING_BALLS_UPDATE_INTERVAL;
}

uint8_t LEDModeBouncingBalls::getBrightness() {
    return brightness;
}

void LEDModeBouncingBalls::setBrightness(uint8_t brightness) {
    LEDModeBouncingBalls::brightness = brightness;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeBouncingBalls::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeBouncingBalls::getHue() {
    return hue;
}

void LEDModeBouncingBalls::setHue(float hue) {
    LEDModeBouncingBalls::hue = hue;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeBouncingBalls::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

float LEDModeBouncingBalls::getSaturation() {
    return saturation;
}

void LEDModeBouncingBalls::setSaturation(float saturation) {
    LEDModeBouncingBalls::saturation = saturation;
    strip->clearEndColorTo(HSIColor(hue, saturation, brightness));
    strip->startAnimation(500, std::bind(&LEDModeBouncingBalls::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeBouncingBalls::bounceBalls() {
    for (int i = 0; i < BOUNCING_BALLS_NUM_BALLS; i++) {
        timeSinceLastBounce[i] =  millis() - clockTimeSinceLastBounce[i];
        height[i] = 0.5 * BOUNCING_BALLS_GRAVITY * pow(timeSinceLastBounce[i]/1000 , 2.0 ) + impactVelocity[i] * timeSinceLastBounce[i]/1000;

        if (height[i] < 0) {
            height[i] = 0;
            impactVelocity[i] = dampening[i] * impactVelocity[i];
            clockTimeSinceLastBounce[i] = millis();

            if (impactVelocity[i] < 0.01) {
                impactVelocity[i] = BOUNCING_BALLS_IMPACT_VELOCITY_START;
            }
        }
        position[i] = round(height[i] * (NUM_LEDS - 1) / BOUNCING_BALLS_START_HEIGHT);
    }

    strip->clearTo(HSIColor(0, 0, 0));
    for (int i = 0 ; i < BOUNCING_BALLS_NUM_BALLS; i++) {
        strip->setPixelColor(position[i], currentTarget);
    }
    strip->show();
}

#endif
