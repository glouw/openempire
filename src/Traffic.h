#pragma once

#include <stdint.h>

#include "Color.h"

typedef struct
{
    int32_t cycles[COLOR_COUNT];
    int32_t setpoint;
}
Traffic;

void Traffic_Print(const Traffic);
