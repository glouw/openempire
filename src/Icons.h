#pragma once

#include "Icon.h"

#include "Motive.h"

#include <stdint.h>

typedef struct
{
    const Icon* icon;
    int32_t count;
}
Icons;

Icons Icons_FromMotive(const Motive, const int32_t age);

bool Icons_IsIndexValid(const Icons, const int32_t index);
