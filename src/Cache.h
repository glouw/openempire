#pragma once

#include "Packet.h"
#include "Color.h"
#include "History.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    Packet packet;
    History history[COLOR_COUNT];
    uint64_t parity[COLOR_COUNT]; // XXX. REQUIRES 2D ARRAY WITH BUFFER TO LOOK AT ALL PARITIES.
    int32_t cycles[COLOR_COUNT];
    int32_t queue_size[COLOR_COUNT];
    int32_t pings[COLOR_COUNT];
    int32_t turn;
    int32_t users_connected;
    int32_t users;
    int32_t seed;
    int32_t control[COLOR_COUNT];
    int32_t integral[COLOR_COUNT];
    int32_t restore_count;
    bool is_stable;
    bool is_out_of_sync;
}
Cache;

Cache Cache_Make(const int32_t users);

void Cache_CheckStability(Cache* const, const int32_t setpoint);

void Cache_CheckParity(Cache* const);

void Cache_ClearPacket(Cache* const);

void Cache_ClearHistory(Cache* const);

int32_t Cache_GetCycleMax(Cache* const);

int32_t Cache_GetCycleMin(Cache* const);

bool Cache_GetGameRunning(Cache* const);

int32_t Cache_GetPingMax(Cache* const);

void Cache_CalculateControl(Cache* const, const int32_t setpoint);
