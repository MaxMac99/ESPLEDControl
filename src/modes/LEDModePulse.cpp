#include <utility>

//
// Created by Max Vissing on 2019-06-24.
//

#include "modes/LEDModePulse.h"

LEDModePulse::LEDModePulse(std::shared_ptr <CRGBSet> leds, LEDAccessory *accessory, bool primary) : LEDMode(std::move(leds),
                                                                                                            accessory,
                                                                                                            primary), brightness(0), hue(0), saturation(0), pulseStep(0) {

}

void LEDModePulse::setup() {
    addNameCharacteristic("Pulse");
    addBrightnessCharacteristic();
    addHueCharacteristic();
    addSaturationCharacteristic();
}

void LEDModePulse::start() {
    HKLOGINFO("Starting Pulse\r\n");
}

void LEDModePulse::update() {
    pulseStep += STEP_SIZE;
    setColor();
}

void LEDModePulse::stop() {
    HKLOGINFO("Stopping Pulse\r\n");
}

unsigned long LEDModePulse::getUpdateInterval() const {
    return UPDATE_INTERVAL;
}

uint8_t LEDModePulse::getBrightness() {
    return brightness / 2.55;
}

void LEDModePulse::setBrightness(uint8_t brightness) {
    LEDModePulse::brightness = brightness * 2.55 + 0.5;
    setColor();
}

float LEDModePulse::getHue() {
    return (hue / 255.0) * 360.0;
}

void LEDModePulse::setHue(float hue) {
    LEDModePulse::hue = (hue / 360.0) * 255.0 + 0.5;
    setColor();
}

float LEDModePulse::getSaturation() {
    return saturation / 2.55;
}

void LEDModePulse::setSaturation(float saturation) {
    LEDModePulse::saturation = saturation * 2.55 + 0.5;
    setColor();
}

void LEDModePulse::setColor() {
    leds->fill_solid(CHSV(hue, saturation, (uint8_t) brightness * float(cubicwave8(pulseStep) / 255.0)));
    FastLED.show();
}
