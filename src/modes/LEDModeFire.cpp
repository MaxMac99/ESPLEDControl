//
// Created by Max Vissing on 2019-06-24.
//

#ifdef MODE_FIRE

#include "modes/LEDModeFire.h"

LEDModeFire::LEDModeFire(std::shared_ptr<LEDStrip> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds), accessory, primary), brightness(100), currentBrightness(100), heats(), isRunning(false) {
    for (unsigned char & heat : heats) {
        heat = 0;
    }
}

void LEDModeFire::setup() {
    addNameCharacteristic("Fire");
    addBrightnessCharacteristic();
}

void LEDModeFire::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    currentBrightness = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
    if (!isRunning) {
        HSIColor color = strip->getPixelColor(index);
        color.intensity = currentBrightness;
        strip->setPixelColor(index, color);
        if (!strip->isAnimating() && index == NUM_LEDS - 1) {
            currentBrightness = brightness;
            isRunning = true;
        }
    }
}

void LEDModeFire::start() {
    HKLOGINFO("Starting Fire\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->startAnimation(500, std::bind(&LEDModeFire::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeFire::update() {
    if (isRunning) {
        fire();
    }
}

void LEDModeFire::stop() {
    HKLOGINFO("Stopping Fire\r\n");
    isRunning = false;
    strip->clearEndColorTo(HSIColor(0, 0, 0));
    strip->clearTo(RGBColor(0, 0, 0));
}

unsigned long LEDModeFire::getUpdateInterval() const {
    return FIRE_UPDATE_INTERVAL;
}

uint8_t LEDModeFire::getBrightness() {
    return brightness;
}

void LEDModeFire::setBrightness(uint8_t brightness) {
    LEDModeFire::brightness = brightness;
    if (isRunning) {
        strip->clearEndColorTo(HSIColor(0, 0, brightness));
        strip->startAnimation(500, std::bind(&LEDModeFire::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
            strip->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5)));
        } else if (t192 > 0x40) {             // middle
            // HKLOGINFO("mid currentBrightness: %d heatramp: %d\r\n", currentBrightness, heatramp);
            strip->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * 255.0f + 0.5), int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5), 0));
        } else {                               // coolestt
            // HKLOGINFO("cool currentBrightness: %d heatramp: %d\r\n", currentBrightness, heatramp);
            strip->setPixelColor(i, RGBColor(int(float(currentBrightness) / 100.0 * float(heatramp) + 0.5), 0, 0));
        }
    }
    strip->show();
}

#endif
