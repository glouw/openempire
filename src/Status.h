#pragma once

#include "Age.h"

#include <stdint.h>

typedef struct
{
    int32_t food;
    int32_t gold;
    int32_t stone;
    int32_t wood;
    int32_t population;
    Age age;
}
Status;
