//
// Created by Max Vissing on 2019-04-26.
//

#ifndef UNIT_TEST

#include <Arduino.h>
#include "LEDHomeKit.h"

LEDHomeKit *homeKit;

#ifdef RESET_PIN
void ICACHE_RAM_ATTR handleReset() {
    if (homeKit) {
        homeKit->handleReset();
    }
}
#endif

void setup() {
    Serial.begin(115200);
    Serial.println();

    homeKit = new LEDHomeKit();
    #ifdef RESET_EEPROM
    homeKit->handleReset();
    #endif
    #ifdef RESET_PAIRINGS
    // HKStorage::resetPairings();
    homeKit->resetPairings();
    #endif

    #ifdef RESET_PIN
    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(RESET_PIN, handleReset, FALLING);
    #endif

    homeKit->setup();
}

void loop() {
    homeKit->update();
}

#endif
