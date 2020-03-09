#pragma once

#include "Config.h"

#include <stdint.h>

typedef struct
{
    int32_t parity[CONFIG_CACHE_HISTORY_COUNT];
    int32_t cycles[CONFIG_CACHE_HISTORY_COUNT];
    int32_t count;
}
History;
