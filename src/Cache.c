#include "Cache.h"

#include "Util.h"
#include "Config.h"

Cache Cache_Init(const int32_t users, const int32_t map_power)
{
    static Cache zero;
    Cache cache = zero;
    cache.users = users;
    cache.map_power = map_power;
    cache.seed = rand();
    return cache;
}

void Cache_CheckStability(Cache* const cache, const int32_t setpoint)
{
    cache->is_stable = setpoint > CONFIG_SOCKETS_THRESHOLD_START;
}

void Cache_CheckParity(Cache* const cache)
{
    if(cache->is_stable)
        for(int32_t j = 0; j < COLOR_COUNT; j++)
        {
            const int32_t cycles_check = cache->cycles[j];
            const int32_t parity_check = cache->parity[j];
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                const int32_t cycles = cache->cycles[i];
                const int32_t parity = cache->parity[i];
                if(cycles == cycles_check && parity != parity_check)
                {
                    cache->is_out_of_sync = true;
                    return;
                }
            }
        }
}

void Cache_Clear(Cache* const cache)
{
    static Packet zero;
    cache->packet = zero;
    cache->turn++;
}

int32_t Cache_GetCycleSetpoint(Cache* const cache)
{
    int32_t setpoint = 0;
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = cache->cycles[i];
        if(cycles > 0)
        {
            setpoint += cycles;
            count++;
        }
    }
    return (count > 0) ? (setpoint / count) : 0;
}

int32_t Cache_GetCycleMax(Cache* const cache)
{
    int32_t max = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = cache->cycles[i];
        if(cycles > max)
            max = cycles;
    }
    return max;
}

bool Cache_GetGameRunning(Cache* const cache)
{
    return cache->users_connected == cache->users;
}

int32_t Cache_GetPingMax(Cache* const cache)
{
    int32_t max = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t ping = cache->pings[i];
        if(ping > max)
            max = ping;
    }
    return max;
}

void Cache_CalculateControlChars(Cache* const cache, const int32_t setpoint)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        cache->control[i] = (cache->cycles[i] < setpoint)
            ? PACKET_CONTROL_SPEED_UP
            : PACKET_CONTROL_STEADY;
}

