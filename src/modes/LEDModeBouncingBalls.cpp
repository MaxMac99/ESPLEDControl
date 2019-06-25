//
// Created by Max Vissing on 2019-06-24.
//

#include "modes/LEDModeBouncingBalls.h"

LEDModeBouncingBalls::LEDModeBouncingBalls(std::shared_ptr <CRGBSet> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds),
                                                                                                          accessory,
                                                                                                          primary), brightness(0), balls(), height(), impactVelocity(), timeSinceLastBounce(), position(), clockTimeSinceLastBounce(), dampening() {
    for (int i = 0 ; i < NUM_BALLS; i++) {
        balls[i] = randomColor();
        clockTimeSinceLastBounce[i] = millis();
        height[i] = START_HEIGHT;
        position[i] = 0;
        impactVelocity[i] = IMPACT_VELOCITY_START;
        timeSinceLastBounce[i] = 0;
        dampening[i] = 0.90 - float(i) / pow(NUM_BALLS, 2);
    }
}

CRGB LEDModeBouncingBalls::randomColor() {
    int color = random(1, 10);
    switch (color) {
        case 1:
            return CRGB::Red;
        case 2:
            return CRGB::Green;
        case 3:
            return CRGB::Blue;
        case 4:
            return CRGB::Yellow;
        case 5:
            return CRGB::Tomato;
        case 6:
            return CRGB::RoyalBlue;
        case 7:
            return CRGB::MediumSpringGreen;
        case 8:
            return CRGB::LightCyan;
        case 9:
            return CRGB::Gold;
        case 10:
            return CRGB::Orange;
    }
    return CRGB(0, 0, 0);
}

void LEDModeBouncingBalls::setup() {
    addNameCharacteristic("Balls");
    addBrightnessCharacteristic();
}

void LEDModeBouncingBalls::start() {
    HKLOGINFO("Starting Bouncing Balls\r\n");
    setBrightness(brightness);
}

void LEDModeBouncingBalls::update() {
    bounceBalls();
}

void LEDModeBouncingBalls::stop() {
    HKLOGINFO("Stopping Bouncing Balls\r\n");
}

unsigned long LEDModeBouncingBalls::getUpdateInterval() const {
    return UPDATE_INTERVAL;
}

uint8_t LEDModeBouncingBalls::getBrightness() {
    return brightness;
}

void LEDModeBouncingBalls::setBrightness(uint8_t brightness) {
    LEDModeBouncingBalls::brightness = brightness;
    uint8_t brightnessConv = LEDAccessory::convertBrightness(brightness, 100);
    FastLED.setBrightness(brightnessConv);
}

void LEDModeBouncingBalls::bounceBalls() {
    for (int i = 0; i < NUM_BALLS; i++) {
        timeSinceLastBounce[i] =  millis() - clockTimeSinceLastBounce[i];
        height[i] = 0.5 * GRAVITY * pow(timeSinceLastBounce[i]/1000 , 2.0 ) + impactVelocity[i] * timeSinceLastBounce[i]/1000;

        if (height[i] < 0) {
            height[i] = 0;
            impactVelocity[i] = dampening[i] * impactVelocity[i];
            clockTimeSinceLastBounce[i] = millis();

            if (impactVelocity[i] < 0.01) {
                impactVelocity[i] = IMPACT_VELOCITY_START;
                balls[i] = randomColor();
            }
        }
        position[i] = round(height[i] * (NUM_LEDS - 1) / START_HEIGHT);
    }

    for (int i = 0 ; i < NUM_BALLS; i++) {
        (*leds)[position[i]] = balls[i];
    }
    FastLED.show();

    leds->fill_solid(CRGB(0, 0, 0));
}
