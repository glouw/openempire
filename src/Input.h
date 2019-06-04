#pragma once

#include "Point.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    // Keyboard state.

    const uint8_t* key;

    // User closed window.

    bool done;

    // Mouse position.

    Point point;

    // Mouse deltas.

    int32_t dx;
    int32_t dy;

    // Mouse button is pushed (Left, Middle, Right):

    int32_t l;
    int32_t m;
    int32_t r;

    // Mouse button was up last frame (Left, Middle, Right).

    int32_t lu;
    int32_t mu;
    int32_t ru;

    // Mouse button was down last frame (Left, Middle, Right).

    int32_t ld;
    int32_t md;
    int32_t rd;

    // Last states (Left, middle, right).

    int32_t ll;
    int32_t lm;
    int32_t lr;

    Point ld_point;
}
Input;

Input Input_Pump(Input);

Input Input_Ready(void);
