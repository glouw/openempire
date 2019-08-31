#pragma once

#include "Point.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    const uint8_t* key;
    bool done;
    Point point;
    int32_t dx;
    int32_t dy;
    // State held.
    int32_t l;
    int32_t m;
    int32_t r;
    // State up.
    int32_t lu;
    int32_t mu;
    int32_t ru;
    // State down.
    int32_t ld;
    int32_t md;
    int32_t rd;
    // State last.
    int32_t ll;
    int32_t lm;
    int32_t lr;
    // Point of last left and right down state.
    Point ld_point;
    Point rd_point;
}
Input;

Input Input_Pump(Input);

Input Input_Ready(void);
