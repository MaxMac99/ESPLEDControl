//
// Created by Max Vissing on 2019-04-26.
//

#ifndef UNIT_TEST

#include <Arduino.h>
#include "LEDHomeKit.h"

LEDHomeKit *homeKit;

void setup() {
    Serial.begin(115200);
    Serial.println();

    Serial.println("Start Setup");
    homeKit = new LEDHomeKit("372-64-103", "P1F3");

    homeKit->setup();
    Serial.println("Finished Setup");
}

void loop() {
    homeKit->update();
}

#endif
