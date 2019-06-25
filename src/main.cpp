//
// Created by Max Vissing on 2019-04-26.
//

#ifndef UNIT_TEST

#include <Arduino.h>
#include "LEDHomeKit.h"

LEDHomeKit *homeKit;

void handleReset() {
    if (homeKit) {
        homeKit->handleReset();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    homeKit = new LEDHomeKit("372-64-103", "P1F3");

    pinMode(D3, INPUT_PULLUP);
    attachInterrupt(D3, handleReset, FALLING);

    homeKit->setup();
}

void loop() {
    homeKit->update();
}

#endif
