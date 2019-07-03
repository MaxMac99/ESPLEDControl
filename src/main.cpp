//
// Created by Max Vissing on 2019-04-26.
//

#ifndef UNIT_TEST

#ifndef HKPASSWORD
#define HKPASSWORD "123-45-678"
#pragma message ("WARNING: Using default Password 123-45-678")
#endif

#ifndef HKSETUPID
#define HKSETUPID "F1HK"
#pragma message ("WARNING: Using default Setup Id F1HK")
#endif

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

    homeKit = new LEDHomeKit(HKPASSWORD, HKSETUPID);

    pinMode(D3, INPUT_PULLUP);
    attachInterrupt(D3, handleReset, FALLING);

    homeKit->setup();
}

void loop() {
    homeKit->update();
}

#endif
