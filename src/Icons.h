#pragma once

#include "Icon.h"

#include "Motive.h"
#include "Age.h"

#include <stdint.h>

typedef struct
{
    IconType icon_type;
    Icon icon;
}
Button;

typedef struct
{
    const Button* button;
    int32_t count;
}
Icons;

Icons Icons_FromMotive(const Motive, const Age);

bool Icons_IsIndexValid(const Icons, const int32_t index);
