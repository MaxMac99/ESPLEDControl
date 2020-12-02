//
// Created by Max Vissing on 2019-04-26.
//

#ifndef UNIT_TEST

#include <Arduino.h>
#include "LEDHomeKit.h"

#ifdef RESET_PIN
void ICACHE_RAM_ATTR handleReset() {
    if (LEDHomeKit::shared()) {
        LEDHomeKit::shared()->handleReset();
    }
}
#endif

void setup() {
    Serial.begin(115200);
    Serial.println();

    #ifdef RESET_EEPROM
    LEDHomeKit::shared()->handleReset();
    #endif
    #ifdef RESET_PAIRINGS
    LEDHomeKit::shared()->resetPairings();
    #endif

    #ifdef RESET_PIN
    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(RESET_PIN, handleReset, FALLING);
    #endif

    LEDHomeKit::shared()->setup();
}

void loop() {
    LEDHomeKit::shared()->update();
}

#endif
