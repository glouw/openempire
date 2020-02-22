#pragma once

#include "Packet.h"
#include "Color.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    Packet packet;
    uint64_t parity[COLOR_COUNT];
    int32_t cycles[COLOR_COUNT];
    int32_t queue_size[COLOR_COUNT];
    int32_t pings[COLOR_COUNT];
    int32_t turn;
    int32_t users_connected;
    int32_t users;
    int32_t seed;
    int32_t map_power;
    bool is_stable;
    bool is_out_of_sync;
    char control[COLOR_COUNT];
}
Cache;

Cache Cache_Init(const int32_t users, const int32_t map_power);

void Cache_CheckStability(Cache* const, const int32_t setpoint);

void Cache_CheckParity(Cache* const);

void Cache_Clear(Cache* const);

int32_t Cache_GetCycleSetpoint(Cache* const);

int32_t Cache_GetCycleMax(Cache* const);

bool Cache_GetGameRunning(Cache* const);

int32_t Cache_GetPingMax(Cache* const);

void Cache_CalculateControlChars(Cache* const, const int32_t setpoint);
