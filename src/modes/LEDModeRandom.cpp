/**
 * @file LEDModeRandom.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-12-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef MODE_RANDOM

#include "modes/LEDModeRandom.h"

LEDModeRandom::LEDModeRandom(LEDAccessory *accessory, bool primary) : LEDMode(accessory, "Random", primary), brightness(100), currentBrightness(100) {
}

LEDModeRandom::~LEDModeRandom() {
    HeatPoint *heatPoint = heatPoints;
    while (heatPoint != nullptr) {
        HeatPoint *copy = heatPoint;
        heatPoint = heatPoint->getNext();
        delete copy;
    }
}

void LEDModeRandom::setup() {
    addBrightnessCharacteristic();
}

void LEDModeRandom::handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param) {
    if (hueAnimationEnabled) {
        HSIColor color = HSIColor::linearBlend<HueBlendClockwiseDirection>(startColor, endColor, param.progress);
        currentBrightness = color.intensity;
        LEDHomeKit::shared()->getStrip()->setPixelColor(index, color);
        if (!LEDHomeKit::shared()->getStrip()->isAnimating() && index == NUM_LEDS - 1) {
            hueAnimationEnabled = false;
            startAllHeatPoints();
        }
    } else {
        currentBrightness = startColor.intensity + (endColor.intensity - startColor.intensity) * param.progress;
    }
}

void LEDModeRandom::start(bool cleanStart) {
    HKLOGINFO("Starting Random\r\n");
    HKLOGDEBUG("start clean %d\r\n", cleanStart);
    if (heatPoints == nullptr) {
        int16_t i = random(1-HEATPOINT_DISTANCE_MIN, 1);
        uint8_t width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
        uint8_t paletteSize = sizeof(palette)/sizeof(palette[0]);
        HeatPoint *prev = new HeatPoint(i, width, palette[random(0, paletteSize)]);
        heatPoints = prev;
        HKLOGDEBUG("[LEDModeRandom::start] first Heatpoint pos: %d width: %d\r\n", i, width);
        i += width;
        while (i < NUM_LEDS) {
            i += random(HEATPOINT_DISTANCE_MIN, HEATPOINT_DISTANCE_MAX+1);
            width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
            HeatPoint *next = new HeatPoint(i, width, palette[random(0, paletteSize)]);
            HKLOGDEBUG("[LEDModeRandom::start] next Heatpoint pos: %d width: %d\r\n", i, width);
            prev->setNext(next);
            prev = next;
            i += width;
        }
    }

    if (cleanStart) {
        HKLOGDEBUG("current Brightness: %d\r\n", currentBrightness);
        startAllHeatPoints();
        LEDHomeKit::shared()->getStrip()->clearEndColorTo(HSIColor(0, 0, brightness));
    } else {
        HeatPoint *currentPoint = heatPoints;
        for (int i = 0; i < NUM_LEDS; i++) {
            if (float(i) >= currentPoint->getCurrentPos() || float(i) <= currentPoint->getCurrentEndPos()) {
                LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, currentPoint->getCurrentColor().asHSI(currentBrightness));
                continue;
            }
            if (i > currentPoint->getCurrentEndPos()) {
                currentPoint = currentPoint->getNext();
            }
            float progress = (float(i) - currentPoint->getCurrentEndPos()) / (currentPoint->getNext()->getCurrentPos() - currentPoint->getCurrentEndPos());
            HSIColor color = currentPoint->getCurrentColor().asHSI(brightness);
            HSIColor nextColor = currentPoint->getNext()->getCurrentColor().asHSI(brightness);
            
            LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, HSIColor::linearBlend<HueBlendShortestDistance>(color, nextColor, progress));
        }
    }
    hueAnimationEnabled = !cleanStart;
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRandom::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeRandom::update() {
    HeatPoint *point = heatPoints;
    if (point != nullptr) {
        while (point->getNext() != nullptr) {
            point->update();
            point = point->getNext();
        }
        point = heatPoints;
        for (int i = 0; i < NUM_LEDS; i++) {
            if (i >= point->getCurrentPos() && i < point->getCurrentEndPos()) {
                HSIColor color = point->getCurrentColor().asHSI(currentBrightness);
                LEDHomeKit::shared()->getStrip()->setPixelColor(i, color);
                continue;
            }
            if (i >= point->getNext()->getCurrentPos()) {
                HSIColor color = point->getNext()->getCurrentColor().asHSI(currentBrightness);
                LEDHomeKit::shared()->getStrip()->setPixelColor(i, color);

                point = point->getNext();
                continue;
            }

            float progress = (float(i) - point->getCurrentEndPos()) / (point->getNext()->getCurrentPos() - point->getCurrentEndPos());
            HSIColor color = point->getCurrentColor().asHSI(currentBrightness);
            HSIColor nextColor = point->getNext()->getCurrentColor().asHSI(currentBrightness);
            
            HSIColor hsiColor = HSIColor::linearBlend<HueBlendShortestDistance>(color, nextColor, progress);
            LEDHomeKit::shared()->getStrip()->setPixelColor(i, hsiColor);
        }
    }
    LEDHomeKit::shared()->getStrip()->show();
}

void LEDModeRandom::stop() {
    HKLOGINFO("Stopping Random\r\n");
    HeatPoint *heatPoint = heatPoints;
    while (heatPoint != nullptr) {
        HeatPoint *copy = heatPoint;
        heatPoint = heatPoint->getNext();
        delete copy;
    }
    heatPoints = nullptr;

    hueAnimationEnabled = true;
    for (int i = 0; i < NUM_LEDS; i++) {
        HSIColor color = LEDHomeKit::shared()->getStrip()->getPixelColor(i);
        color.intensity = 0;
        LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, color);
    }
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRandom::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

unsigned long LEDModeRandom::getUpdateInterval() const {
    return RANDOM_UPDATE_INTERVAL;
}

uint8_t LEDModeRandom::getBrightness() {
    return brightness;
}

void LEDModeRandom::setBrightness(uint8_t brightness) {
    LEDMode::setBrightness(brightness);
    HKLOGDEBUG("[LEDModeRainbowCycle::setBrightness] set brightness to %u\r\n", brightness);
    LEDModeRandom::brightness = brightness;
    for (int i = 0; i < NUM_LEDS; i++) {
        LEDHomeKit::shared()->getStrip()->setEndColorPixel(i, HSIColor(0, 0, brightness));
    }
    LEDHomeKit::shared()->getStrip()->startAnimation(500, std::bind(&LEDModeRandom::handleAnimation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void LEDModeRandom::startAllHeatPoints() {
    HKLOGDEBUG("startAllHeatPoints\r\n");
    if (heatPoints == nullptr) {
        return;
    }

    uint8_t paletteSize = sizeof(palette)/sizeof(palette[0]);

    HeatPoint *currentPoint = heatPoints;
    int16_t i = random(1-HEATPOINT_DISTANCE_MIN, 1);
    uint8_t width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);

    HKLOGDEBUG("[LEDModeRandom::startAllHeatPoints] first Heatpoint pos: %d width: %d\r\n", i, width);

    HSColor color = palette[random(0, paletteSize)];
    uint8_t j = 0;
    while (!isColorAllowed(color, currentPoint, nullptr) && j <= paletteSize) {
        color = palette[j++];
    }

    currentPoint->start(i, width, color, random(HEATPOINT_DURATION_MIN, HEATPOINT_DURATION_MAX+1)*TIME_FACTOR, std::bind(&LEDModeRandom::restartHeatPoint, this, std::placeholders::_1));

    HeatPoint *previousPoint = currentPoint;
    currentPoint = currentPoint->getNext();

    while (i < NUM_LEDS) {
        i += random(HEATPOINT_DISTANCE_MIN, HEATPOINT_DISTANCE_MAX+1);
        width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
    
        HKLOGDEBUG("[LEDModeRandom::startAllHeatPoints] next Heatpoint pos: %d width: %d\r\n", i, width);

        int16_t posBefore = currentPoint->getEndPos();
        color = palette[random(0, paletteSize)];
        uint8_t j = 0;
        while (!isColorAllowed(color, currentPoint, previousPoint) && j <= paletteSize) {
            color = palette[j++];
        }

        currentPoint->start(i, width, color, random(HEATPOINT_DURATION_MIN, HEATPOINT_DURATION_MAX+1)*TIME_FACTOR, std::bind(&LEDModeRandom::restartHeatPoint, this, std::placeholders::_1));
        i += width;

        if (i < NUM_LEDS && currentPoint->getNext() == nullptr) {
            width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
            HeatPoint *next = new HeatPoint(posBefore, width, palette[random(0, paletteSize)]);
            HKLOGDEBUG("[LEDModeRandom::startAllHeatPoints] new Heatpoint pos: %d width: %d\r\n", posBefore, width);
            currentPoint->setNext(next);
        }

        previousPoint = currentPoint;
        currentPoint = currentPoint->getNext();
    }
    if (currentPoint != nullptr && currentPoint->getNext() != nullptr) {
        HeatPoint *current = heatPoints;
        while (current->getNext() == currentPoint) {
            current = current->getNext();
        }
        current->setNext(nullptr);
    }
    while (currentPoint != nullptr && currentPoint->getNext() != nullptr) {
        HeatPoint *current = currentPoint;
        currentPoint = currentPoint->getNext();
        HKLOGDEBUG("[LEDModeRandom::startAllHeatPoints] delete Heatpoint\r\n");
        delete current;
    }
}

void LEDModeRandom::restartHeatPoint(HeatPoint *point) {
    if (heatPoints == nullptr) {
        return;
    }

    uint8_t paletteSize = sizeof(palette)/sizeof(palette[0]);
    if (heatPoints == point) {
        int16_t i = random(1-HEATPOINT_DISTANCE_MIN, 1);
        uint8_t width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);

        HSColor color = palette[random(0, paletteSize)];
        uint8_t j = 0;
        while (!isColorAllowed(color, point, nullptr) && j <= paletteSize) {
            color = palette[j++];
        }
        point->start(i, width, color, random(HEATPOINT_DURATION_MIN, HEATPOINT_DURATION_MAX+1)*TIME_FACTOR, std::bind(&LEDModeRandom::restartHeatPoint, this, std::placeholders::_1));
        return;
    }

    HeatPoint *prev = heatPoints;
    while (prev->getNext() != point) {
        prev = prev->getNext();
    }

    if (point->getNext() == nullptr && prev->getCurrentEndPos() >= NUM_LEDS && prev->getCurrentEndPos() >= NUM_LEDS) {
        HKLOGDEBUG("[LEDModeRandom::restartHeatPoint] delete Heatpoint\r\n");
        prev->setNext(nullptr);
        delete point;
        return;
    }

    uint8_t width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
    int16_t minPos = max(prev->getEndEndPos(), (int16_t) prev->getCurrentEndPos()) + HEATPOINT_DISTANCE_MIN;
    int16_t maxPos = min(prev->getEndEndPos(), (int16_t)(prev->getCurrentEndPos()+1)) + HEATPOINT_DISTANCE_MAX;
    if (point->getNext() != nullptr) {
        int16_t nextMinPos = min(point->getNext()->getEndPos(), (int16_t)(point->getNext()->getCurrentPos())) - HEATPOINT_DISTANCE_MIN;
        if (maxPos + width > nextMinPos) {
            maxPos = nextMinPos - width;
        }
    }
    int16_t nextPos = random(minPos, maxPos+1);
    HSColor color = palette[random(0, paletteSize)];
    uint8_t j = 0;
    while (!isColorAllowed(color, point, prev) && j <= paletteSize) {
        color = palette[j++];
    }
    point->start(nextPos, width, color, random(HEATPOINT_DURATION_MIN, HEATPOINT_DURATION_MAX+1)*TIME_FACTOR, std::bind(&LEDModeRandom::restartHeatPoint, this, std::placeholders::_1));
    
    if (point->getNext() != nullptr) {
        int16_t nextMaxPos = max(point->getNext()->getEndPos(), (int16_t)(point->getNext()->getCurrentPos()+1));
        if (nextMaxPos - nextPos - width > HEATPOINT_DISTANCE_MAX) {
            HKLOGDEBUG("[LEDModeRandom::restartHeatPoint] new Heatpoint\r\n");
            int16_t newPos = random(nextPos + width + HEATPOINT_DISTANCE_MIN, nextMaxPos - HEATPOINT_DISTANCE_MIN+1);
            HeatPoint *newPoint = new HeatPoint(newPos, 1, HSColor::fromHSIColor(LEDHomeKit::shared()->getStrip()->getPixelColor(newPos)));
            newPoint->setNext(point->getNext());
            point->setNext(newPoint);

            newPos = random(nextPos + width + HEATPOINT_DISTANCE_MIN, nextMaxPos - HEATPOINT_DISTANCE_MIN+1);
            width = random(HEATPOINT_WIDTH_MIN, HEATPOINT_WIDTH_MAX+1);
            color = palette[random(0, paletteSize)];
            uint8_t j = 0;
            while (!isColorAllowed(color, newPoint, point) && j <= paletteSize) {
                color = palette[j++];
            }
            newPoint->start(newPos, width, color, random(HEATPOINT_DURATION_MIN, HEATPOINT_DURATION_MAX+1)*TIME_FACTOR, std::bind(&LEDModeRandom::restartHeatPoint, this, std::placeholders::_1));
        }
    }
}

int16_t LEDModeRandom::calculateDegreeDistance(uint16_t alpha, uint16_t beta) {
    return (360 + ((beta - alpha + 180) % 360)) % 360 - 180;
}

bool LEDModeRandom::isDegreeAllowed(uint16_t alpha, uint16_t beta, uint16_t gamma, uint16_t target) {
    int16_t ab = calculateDegreeDistance(alpha, beta);
    int16_t ac = calculateDegreeDistance(alpha, gamma);
    int16_t at = calculateDegreeDistance(alpha, target);

    int16_t minDegree = -180;
    int16_t maxDegree = 180;
    if (ab <= 0 && ac <= 0) {
        minDegree = -180;
        maxDegree = 180 + min(ab, ac);
    } else if (ab >= 0 && ac >= 0) {
        minDegree = -180 + max(ab, ac);
        maxDegree = 180;
    } else if (ab < 0 && ac > 0) {
        minDegree = -180 + ac;
        maxDegree = 180 + ab;
    } else if (ab > 0 && ac < 0) {
        minDegree = -180 + ab;
        maxDegree = 180 + ac;
    }
    if (at < 0) {
        return at >= minDegree;
    }
    return at <= maxDegree;
}

bool LEDModeRandom::isColorAllowed(HSColor color, HeatPoint *current, HeatPoint *previous) {
    if (previous != nullptr && !isDegreeAllowed(previous->getCurrentColor().hue, previous->getEndColor().hue, current->getEndColor().hue, color.hue)) {
        return false;
    }
    if (current->getNext() != nullptr && !isDegreeAllowed(current->getNext()->getCurrentColor().hue, current->getNext()->getEndColor().hue, current->getEndColor().hue, color.hue)) {
        return false;
    }
    return true;
}

#endif
