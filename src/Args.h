#pragma once

#include "Color.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    bool demo;
    bool measure;
    const char* path;
    Color color;
}
Args;

Args Args_Parse(int32_t argc, char* argv[]);
