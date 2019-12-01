#pragma once

#include "Color.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* path;
    Color color;
    bool is_server;
    const char* host;
    int32_t port;
}
Args;

Args Args_Parse(const int32_t argc, const char* argv[]);
