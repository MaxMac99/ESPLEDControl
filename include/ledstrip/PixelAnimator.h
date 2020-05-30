/**
 * @file PixelAnimator.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LED_LIGHTS_ANIMATOR_H
#define LED_LIGHTS_ANIMATOR_H

#include <Arduino.h>
#include "HueBlend.h"

enum AnimationState {
    AnimationState_Started,
    AnimationState_Progress,
    AnimationState_Completed
};

struct AnimationParam {
    float progress;
    AnimationState state;
};

typedef std::function<void(const AnimationParam &param)> AnimationUpdateCallback;

#define MILLISECONDS        1    // ~65 seconds max duration, ms updates
#define CENTISECONDS       10    // ~10.9 minutes max duration, centisecond updates
#define DECISECONDS       100    // ~1.8 hours max duration, decisecond updates
#define SECONDS          1000    // ~18.2 hours max duration, second updates
#define DECASECONDS     10000    // ~7.5 days, 10 second updates


class PixelAnimator {
public:
    PixelAnimator(uint16_t timeScale = MILLISECONDS);
    ~PixelAnimator() = default;

    bool isAnimating() {
        return _isAnimating;
    };
    void startAnimation(uint16_t duration, AnimationUpdateCallback animUpdate);
    void stopAnimation();
    void updateAnimation();
private:
    uint32_t _animationLastTick;
    uint16_t _timeScale;
    uint16_t _duration;
    uint16_t _remaining;
    AnimationUpdateCallback _fnCallback;
    bool _isAnimating;
};

#endif
