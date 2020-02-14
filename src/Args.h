#pragma once

#include "Civ.h"
#include "Color.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* path;
    Color color;
    Civ civ;
    bool is_server;
    const char* host;
    int32_t port;
    int32_t port_ping;
    int32_t xres;
    int32_t yres;
    int32_t users;
    int32_t map_power;
    bool quiet;
    bool demo;
}
Args;

Args Args_Parse(const int32_t argc, const char* argv[]);
