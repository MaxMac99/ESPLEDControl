//
// Created by Max Vissing on 2019-06-23.
//

#ifndef LED_HAP_ESP8266_ALLLEDMODES_H
#define LED_HAP_ESP8266_ALLLEDMODES_H

#include <Arduino.h>

#include "LEDModeStatic.h"
#include "LEDModeRainbowCycle.h"
#include "LEDModePulse.h"
#include "LEDModeFire.h"

inline std::vector<LEDMode *> generateModes(const std::shared_ptr<CRGBSet>& leds, LEDAccessory *accessory) {
    std::vector<LEDMode *> modes;
    modes.push_back(new LEDModeStatic(leds, accessory, true));
    modes.push_back(new LEDModeRainbowCycle(leds, accessory));
    modes.push_back(new LEDModePulse(leds, accessory));
    modes.push_back(new LEDModeFire(leds, accessory));
    return modes;
}

#endif //LED_HAP_ESP8266_ALLLEDMODES_H
