#pragma once

#include "Point.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    bool done;
    const uint8_t* key;
    Point cursor;
    int32_t l;
    int32_t r;
    int32_t lu; // Up.
    int32_t ru;
    int32_t ld; // Down.
    int32_t rd;
    int32_t ll; // Last.
    int32_t lr;
}
Input;

Input Input_Pump(Input);

Input Input_Ready(void);
