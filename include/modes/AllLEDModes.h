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

inline std::vector<LEDMode *> generateModes(LEDAccessory *accessory) {
    std::vector<LEDMode *> modes;

    #ifdef MODE_STATIC
    modes.push_back(new LEDModeStatic(accessory, true));
    #endif

    #ifdef MODE_RAINBOW_CYCLE
    modes.push_back(new LEDModeRainbowCycle(accessory));
    #endif

    #ifdef MODE_FADE
    modes.push_back(new LEDModeFade(accessory));
    #endif

    #ifdef MODE_PULSE
    modes.push_back(new LEDModePulse(accessory));
    #endif

    #ifdef MODE_FIRE
    modes.push_back(new LEDModeFire(accessory));
    #endif

    #ifdef MODE_BOUNCING_BALLS
    modes.push_back(new LEDModeBouncingBalls(accessory));
    #endif
    return modes;
}

#endif //LED_HAP_ESP8266_ALLLEDMODES_H
