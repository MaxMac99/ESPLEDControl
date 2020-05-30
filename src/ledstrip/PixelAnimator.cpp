/**
 * @file PixelAnimator.cpp
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "ledstrip/PixelAnimator.h"

PixelAnimator::PixelAnimator(uint16_t timeScale) : _animationLastTick(0), _duration(0), _remaining(0), _fnCallback(NULL), _isAnimating(false) {
    _timeScale = (timeScale < 1) ? 1 : timeScale > 32768 ? 32768 : timeScale;
}

void PixelAnimator::startAnimation(uint16_t duration, AnimationUpdateCallback animationUpdate) {
    _animationLastTick = millis();
    stopAnimation();
    if (duration == 0) {
        duration = 1;
    }

    _duration = duration;
    _remaining = duration;
    _fnCallback = animationUpdate;

    _isAnimating = true;
}

void PixelAnimator::stopAnimation() {
    if (_isAnimating) {
        _isAnimating = false;
        _remaining = 0;
    }
}

void PixelAnimator::updateAnimation() {
    uint32_t currentTick = millis();
    uint32_t delta = currentTick - _animationLastTick;
    if (delta >= _timeScale) {
        delta /= _timeScale;
        
        AnimationParam param;

        if (_remaining > delta) {
            param.state = _remaining == _duration ? AnimationState_Started : AnimationState_Progress;
            param.progress = (float)(_duration - _remaining) / (float)_duration;
            
            _fnCallback(param);

            _remaining -= delta;
        } else if (_remaining > 0) {
            param.state = AnimationState_Completed;
            param.progress = 1.0f;

            _remaining = 0;
            _isAnimating = false;

            _fnCallback(param);
        }

        _animationLastTick = currentTick;
    }
}
