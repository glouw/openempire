#pragma once

#include "Parity.h"
#include "Config.h"
#include "Backup.h"
#include "Packet.h"
#include "Color.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    Parity parity[COLOR_COUNT][CONFIG_MAIN_LOOP_SPEED_FPS]; // BUFFER IS LARGE ENOUGH TO SUPPORT ONE SECOND OF STREAMING.
    Parity panic[COLOR_COUNT][BACKUP_MAX];
    Packet packet;
    int32_t queue_size[COLOR_COUNT];
    int32_t pings[COLOR_COUNT];
    int32_t turn;
    int32_t seed;
    int32_t users_connected;
    int32_t users;
    int32_t map_power;
    int32_t panic_count;
    char control[COLOR_COUNT];
    bool is_out_of_sync;
    bool is_stable;
}
Cache;

Cache Cache_Init(const int32_t users, const int32_t map_power);

void Cache_AppendParity(Cache*, const Color color, const Parity);

void Cache_CalcOutOfSync(Cache* const);

void Cache_Clear(Cache* const);

int32_t Cache_GetCycleSetpoint(Cache* const);

int32_t Cache_GetCycleMax(Cache* const);

int32_t Cache_GetPingMax(Cache* const);

void Cache_CalculateControlChars(Cache* const, const int32_t setpoint);

void Cache_CalcStability(Cache* const, const int32_t setpoint);

void Cache_DumpPanicTable(Cache* const);
