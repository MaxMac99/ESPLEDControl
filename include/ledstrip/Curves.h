/**
 * @file Curves.h
 * @author Max Vissing (max_vissing@yahoo.de)
 * @brief 
 * @version 0.1
 * @date 2020-05-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LED_LIGHTS_CURVES_H
#define LED_LIGHTS_CURVES_H

#include <Arduino.h>

static inline uint8_t curveTriwave8(uint8_t in)
{
    if( in & 0x80) {
        in = 255 - in;
    }
    uint8_t out = in << 1;
    return out;
}

static inline uint8_t curveScale8_LEAVING_R1_DIRTY(uint8_t i, uint8_t scale) {
    return (((uint16_t)i) * ((uint16_t)(scale)+1)) >> 8;
}

static inline uint8_t curveEase8InOutCubic(uint8_t i) {
    uint8_t ii  = curveScale8_LEAVING_R1_DIRTY(i, i);
    uint8_t iii = curveScale8_LEAVING_R1_DIRTY(ii, i);

    uint16_t r1 = (3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii));

    /* the code generated for the above *'s automatically
       cleans up R1, so there's no need to explicitily call
       cleanup_R1(); */

    uint8_t result = r1;

    // if we got "256", return 255:
    if (r1 & 0x100) {
        result = 255;
    }
    return result;
}

static inline uint8_t curveCubicwave8(uint8_t in) {
    return curveEase8InOutCubic(curveTriwave8(in));
}

#endif
