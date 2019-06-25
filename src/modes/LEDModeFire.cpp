//
// Created by Max Vissing on 2019-06-24.
//

#include "modes/LEDModeFire.h"

LEDModeFire::LEDModeFire(std::shared_ptr <CRGBSet> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds),
                                                                                                            accessory,
                                                                                                            primary), brightness(0), heats() {
    for (unsigned char & heat : heats) {
        heat = 0;
    }
}

void LEDModeFire::setup() {
    addNameCharacteristic("Fire");
    addBrightnessCharacteristic();
}

void LEDModeFire::start() {
    HKLOGINFO("Starting Fire\r\n");
    setBrightness(brightness);
}

void LEDModeFire::update() {
    fire();
}

void LEDModeFire::stop() {
    HKLOGINFO("Stopping Fire\r\n");
}

unsigned long LEDModeFire::getUpdateInterval() const {
    return UPDATE_INTERVAL;
}

uint8_t LEDModeFire::getBrightness() {
    return brightness;
}

void LEDModeFire::setBrightness(uint8_t brightness) {
    LEDModeFire::brightness = brightness;
    uint8_t brightnessConv = LEDAccessory::convertBrightness(brightness, 100);
    FastLED.setBrightness(brightnessConv);
}

void LEDModeFire::fire() {
    int cooldown;
    for (unsigned char & heat : heats) {
        cooldown = random(0, ((COOLING * 10) / NUM_LEDS) + 2);

        if(cooldown > heat) {
            heat = 0;
        } else {
            heat = heat - cooldown;
        }
    }

    for (int k= NUM_LEDS - 1; k >= 2; k--) {
        heats[k] = (heats[k - 1] + heats[k - 2] + heats[k - 2]) / 3;
    }

    if (random(255) < SPARKING ) {
        int y = random(7);
        heats[y] = heats[y] + random(160,255);
        //heat[y] = random(160,255);
    }

    int i = 0;
    for (CRGB &pixel : *leds) {
        byte t192 = lround((heats[i] / 255.0)*191);

        // calculate ramp up from
        byte heatramp = t192 & 0x3F; // 0..63
        heatramp <<= 2; // scale up to 0..252

        // figure out which third of the spectrum we're in:
        if( t192 > 0x80) {                     // hottest
            pixel.setRGB(255, 255, heatramp);
        } else if( t192 > 0x40 ) {             // middle
            pixel.setRGB(255, heatramp, 0);
        } else {                               // coolest
            pixel.setRGB(heatramp, 0, 0);
        }
        i++;
    }
    FastLED.show();
}
