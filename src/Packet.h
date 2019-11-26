#pragma once

#include <stdbool.h>

#include "Overview.h"
#include "Color.h"

typedef struct
{
    Overview overview[COLOR_COUNT];
}
Packet;
