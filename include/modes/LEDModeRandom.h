/**
 * @file LEDModeRandom.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-12-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */



#ifdef MODE_RANDOM

#ifndef LED_HAP_ESP8266_LEDMODE_RANDOM_H
#define LED_HAP_ESP8266_LEDMODE_RANDOM_H

#include "ledstrip/HueBlend.h"
#include "../LEDMode.h"

#ifndef RANDOM_UPDATE_INTERVAL
#define RANDOM_UPDATE_INTERVAL 35
#endif

#define TIME_FACTOR 1

#define HEATPOINT_DISTANCE_MIN 20
#define HEATPOINT_DISTANCE_MAX 30
#define HEATPOINT_WIDTH_MIN 1
#define HEATPOINT_WIDTH_MAX 3
#define HEATPOINT_DURATION_MIN 4000
#define HEATPOINT_DURATION_MAX 6000

#define COLOR_PALETTE {145, 94}, {45, 96}, {329, 96}, {206, 100}, {87, 99}

struct HSColor {
    uint16_t hue;
    uint8_t saturation;

    static HSColor fromHSIColor(HSIColor color) {
        return { color.hue, color.saturation };
    }
    HSIColor asHSI(uint8_t intensity) {
        return HSIColor(hue, saturation, intensity);
    };
};

const static HSColor palette[] = { COLOR_PALETTE };

class HeatPoint {
public:
    HeatPoint(uint16_t pos, uint8_t width, const HSColor &color) : currentPos(pos), endPos(pos), currentWidth(width), endWidth(width), currentColor(std::move(color)), endColor(std::move(color)), next(nullptr) {};

    void start(uint16_t targetPos, uint8_t targetWidth, const HSColor &targetColor, uint16_t duration, std::function<void(HeatPoint *)> callback) {
        startHue = endColor.hue;
        currentPos = endPos;
        currentWidth = endWidth;
        currentColor = endColor;
        endPos = targetPos;
        endWidth = targetWidth;
        endColor = std::move(targetColor);
        
        previousProgress = 0.0;

        this->duration = duration;
        this->callback = callback;
        remaining = duration;
        lastUpdate = millis();
    };
    void update() {
        uint32_t current = millis();
        uint32_t delta = current - lastUpdate;
        
        if (remaining > delta) {
            remaining -= delta;
            float totalProgress = (float)(duration - remaining) / (float)duration;
            float progress = totalProgress - previousProgress;

            currentPos = (float) currentPos + (float)(endPos - currentPos) * progress;
            currentWidth = (float) currentWidth + (float)(endWidth - currentWidth) * progress;

            currentColor.hue = int(HueBlendShortestDistance::HueBlend(float(startHue) / 360, float(endColor.hue) / 360, totalProgress) * 360);
            currentColor.saturation = currentColor.saturation + (endColor.saturation - currentColor.saturation) * progress;

            previousProgress = totalProgress;
        } else {
            remaining = 0;

            currentPos = (float) endPos;
            currentWidth = (float) endWidth;
            currentColor = endColor;

            callback(this);
        }

        lastUpdate = current;
    };
    HSColor getCurrentColor() {
        return currentColor;
    };
    HSColor getEndColor() {
        return endColor;
    }
    int16_t getEndPos() {
        return endPos;
    }
    uint8_t getEndWidth() {
        return endWidth;
    }
    int16_t getEndEndPos() {
        return endPos + endWidth;
    }
    float getCurrentPos() {
        return currentPos;
    }
    float getCurrentEndPos() {
        return currentPos + currentWidth;
    }
    HeatPoint *getNext() {
        return next;
    }
    void setNext(HeatPoint *next) {
        this->next = next;
    }
private:
    float currentPos;
    int16_t endPos;
    float currentWidth;
    uint8_t endWidth;
    uint16_t startHue;
    HSColor currentColor = HSColor();
    HSColor endColor;
    uint16_t duration;
    float previousProgress;
    int32_t lastUpdate = 0;
    uint16_t remaining = 0;
    std::function<void(HeatPoint *)> callback;
    HeatPoint *next;
};

class LEDModeRandom : public LEDMode {
public:
    explicit LEDModeRandom(LEDAccessory *accessory, bool primary=false);
    ~LEDModeRandom();
    void setup() override;
    void start(bool cleanStart) override;
    void update() override;
    void stop() override;
    unsigned long getUpdateInterval() const override;
    uint8_t getBrightness() override;
    void setBrightness(uint8_t brightness) override;
    void handleAnimation(const uint16_t index, const HSIColor &startColor, const HSIColor &endColor, const AnimationParam &param);
private:
    void startAllHeatPoints();
    void restartHeatPoint(HeatPoint *point);
    int16_t calculateDegreeDistance(uint16_t alpha, uint16_t beta);
    bool isDegreeAllowed(uint16_t alpha, uint16_t beta, uint16_t gamma, uint16_t target);
    bool isColorAllowed(HSColor color, HeatPoint *current, HeatPoint *previous);
private:
    float brightness;
    float currentBrightness;
    HeatPoint *heatPoints;
    bool hueAnimationEnabled;
};

#endif //LED_HAP_ESP8266_LEDMODE_RANDOM_H

#endif

