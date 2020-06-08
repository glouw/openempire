#pragma once

#include "Type.h"

#include <stdint.h>

typedef struct
{
    Type type;
    int32_t amount;
}
Resource;

Resource Resource_None(void);
