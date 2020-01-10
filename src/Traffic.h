#pragma once

#include <stdint.h>

#include "Color.h"
#include "Packet.h"

typedef struct
{
    int32_t cycles[COLOR_COUNT];
    int32_t setpoint;
}
Traffic;

Traffic Traffic_Make(const Packet);
