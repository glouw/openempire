#pragma once

#include "Color.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* path;
    Color color;
    bool should_measure;
    bool is_server;
}
Args;

Args Args_Parse(const int32_t argc, const char* argv[]);
