//
// Created by Max Vissing on 2019-06-23.
//

#ifndef LED_HAP_ESP8266_ALLLEDMODES_H
#define LED_HAP_ESP8266_ALLLEDMODES_H

#include <Arduino.h>

#include "LEDModeStatic.h"
#include "LEDModeRainbowCycle.h"
#include "LEDModeFade.h"
#include "LEDModePulse.h"
#include "LEDModeFire.h"
#include "LEDModeBouncingBalls.h"

inline std::vector<LEDMode *> generateModes(const std::shared_ptr<LEDStrip> leds, LEDAccessory *accessory) {
    std::vector<LEDMode *> modes;

    #ifdef MODE_STATIC
    modes.push_back(new LEDModeStatic(leds, accessory, true));
    #endif

    #ifdef MODE_RAINBOW_CYCLE
    modes.push_back(new LEDModeRainbowCycle(leds, accessory));
    #endif

    #ifdef MODE_FADE
    modes.push_back(new LEDModeFade(leds, accessory));
    #endif

    #ifdef MODE_PULSE
    modes.push_back(new LEDModePulse(leds, accessory));
    #endif

    #ifdef MODE_FIRE
    modes.push_back(new LEDModeFire(leds, accessory));
    #endif

    #ifdef MODE_BOUNCING_BALLS
    modes.push_back(new LEDModeBouncingBalls(leds, accessory));
    #endif
    return modes;
}

#endif //LED_HAP_ESP8266_ALLLEDMODES_H
