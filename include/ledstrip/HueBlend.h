/**
 * @file HueBlend.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LED_LIGHTS_HUEBLEND_H
#define LED_LIGHTS_HUEBLEND_H

class HueBlendBase {
protected:
    static float FixWrap(float value) {
        if (value < 0.0f) {
            return value + 1.0f;
        } else if (value > 1.0f) {
            return value - 1.0f;
        }
        return value;
    }
};

class HueBlendShortestDistance : HueBlendBase {
public:
    static float HueBlend(float left, float right, float progress) {
        float delta = right - left;
        float base = left;
        if (delta > 0.5f) {
            base = right;
            delta = 1.0f - delta;
            progress = 1.0f - progress;
        } else if (delta < -0.5f) {
            delta = 1.0f + delta;
        }
        return FixWrap(base + delta * progress);
    }
};

class HueBlendLongestDistance : HueBlendBase
{
public:
    static float HueBlend(float left, float right, float progress) {
        float delta = right - left;
        float base = left;
        if (delta < 0.5f && delta >= 0.0f) {
            base = right;
            delta = 1.0f - delta;
            progress = 1.0f - progress;
        } else if (delta > -0.5f && delta < 0.0f) {
            delta = 1.0f + delta;
        }
        return FixWrap(base + delta * progress);
    };
};

class HueBlendClockwiseDirection : HueBlendBase
{
public:
    static float HueBlend(float left, float right, float progress) {
        float delta = right - left;
        float base = left;
        if (delta < 0.0f) {
            delta = 1.0f + delta;
        }
        return FixWrap(base + delta * progress);
    };
};

class BlendCounterClockwiseDirection : HueBlendBase {
public:
    static float HueBlend(float left, float right, float progress) {
        float delta = right - left;
        float base = left;
        if (delta > 0.0f) {
            delta = delta - 1.0f;
        }
        return FixWrap(base + delta * progress);
    };
};

#endif
