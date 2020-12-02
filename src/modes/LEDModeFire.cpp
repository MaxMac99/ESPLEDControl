//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_FIRE

#include "modes/LEDModeFire.h"

LEDModeFire::LEDModeFire(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Fire", primary), brightness(100), currentBrightness(0), startBrightness(0), heats(), isRunning(false) {
    for (unsigned char & heat : heats) {
        heat = 0;
    }
}

void LEDModeFire::setup() {
    addBrightnessCharacteristic();
}

void LEDModeFire::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (isRunning) {
        currentBrightness = startBrightness + (brightness - startBrightness) * param.progress;
        HKLOGDEBUG("[Fire::handleAnimation] current: %u, start: %u, end: %u\r\n", currentBrightness, startBrightness, brightness);
    } else {
        HSIColor color = LEDHomeKit::shared()->getStrip()->getPixelColor(index);
        color.intensity = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
        LEDHomeKit::shared()->getStrip()->setPixelColor(index, color);
        if (!LEDHomeKit::shared()->getStrip()->isAnimating() && index == NUM_LEDS - 1) {
            currentBrightness = brightness;
            isRunning = true;
        }
    }
}

void LEDModeFire::start() {
    HKLOGINFO("Starting Fire\r\n");
    isRunning = false;
    for (unsigned char & heat : heats) {
        heat = 0;
    }
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeFire::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeFire::update() {
    if (isRunning) {
        fire();
    }
}

void LEDModeFire::stop() {
    HKLOGINFO("Stopping Fire\r\n");
    isRunning = false;
    LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, 0));
    LEDHomeKit::shared()->getStrip()->clearTo(RGBColor(0, 0, 0));
}

unsigned long LEDModeFire::getUpdateInterval() const {
    return FIRE_UPDATE_INTERVAL;
}

uint8_t LEDModeFire::getBrightness() {
    return brightness;
}

void LEDModeFire::setBrightness(uint8_t brightness) {
    LEDMode::setBrightness(brightness);
    startBrightness = currentBrightness;
    LEDModeFire::brightness = brightness;
    HKLOGDEBUG("[Fire::setBrightness] to: %u\r\n", brightness);
    if (isRunning) {
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, brightness));
        LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeFire::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

void LEDModeFire::fire() {
    int cooldown;
    for (unsigned char & heat : heats) {
        cooldown = random(0, ((FIRE_COOLING * 10) / NUM_LEDS) + 2);

        if(cooldown > heat) {
            heat = 0;
        } else {
            heat = heat - cooldown;
        }
    }

    for (int k= NUM_LEDS - 1; k >= 2; k--) {
        heats[k] = (heats[k - 1] + heats[k - 2] + heats[k - 2]) / 3;
    }

    if (random(255) < FIRE_SPARKING) {
        int y = random(7);
        heats[y] = heats[y] + random(160,255);
        //heat[y] = random(160,255);
    }

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        byte t192 = lround((heats[i] / 255.0)*191);

        // calculate ramp up from
        byte heatramp = t192 & 0x3F; // 0..63
        heatramp <<= 2; // scale up to 0..252

        // figure out which third of the spectrum we're in:
        if (t192 > 0x80) {                     // hottest
            // HKLOGINFO("hot currentBrightness: %d heatramp: %d\r\n", currentBrightness, heatramp);
            #ifdef RGBW_COLORS
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBWColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), 0, 0, int(float(currentBrightness) / 100.0 * float(heatramp) * 0.5 + 123.5)));
            #else
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5)));
            #endif
        } else if (t192 > 0x40) {             // middle
            // HKLOGINFO("mid currentBrightness: %d heatramp: %d\r\n", currentBrightness, heatramp);
            #ifdef RGBW_COLORS
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBWColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), 0, 0, int(float(currentBrightness) / 100.0 * float(heatramp) * 0.5 + 0.5)));
            #else
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5), 0));
            #endif
        } else {                               // coolestt
            // HKLOGINFO("cool currentBrightness: %d heatramp: %d\r\n", currentBrightness, heatramp);
            #ifdef RGBW_COLORS
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBWColor(int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5), 0, 0, 0));
            #else
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5), 0, 0));
            #endif
        }
    }
    LEDHomeKit::shared()->getStrip()->show();
}

#endif
