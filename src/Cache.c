#include "Cache.h"

#include "Util.h"
#include "Config.h"

Cache Cache_Make(const int32_t users, const int32_t map_size)
{
    static Cache zero;
    Cache cache = zero;
    cache.users = users;
    cache.map_size = map_size;
    cache.seed = rand();
    return cache;
}

void Cache_CheckStability(Cache* const cache, const int32_t setpoint)
{
    cache->is_stable = setpoint > CONFIG_SOCKETS_THRESHOLD_START;
}

static bool MisMatch(Cache* const cache, const int32_t cycles_check, const int32_t parity_check)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const History history = cache->history[i];
        for(int32_t j = 0; j < history.count; j++)
        {
            const int32_t cycles = cache->cycles[i];
            const int32_t parity = cache->parity[i];
            if(cycles == cycles_check
            && parity != parity_check)
                return true;
        }
    }
    return false;
}

void Cache_CheckParity(Cache* const cache) // XXX. CHECKS SHOULD HAPPEN ON EVERY INCOMING CYCLE AND PARITY, NOT THE LAST ONE FOR EVERY TURN.
{
    if(cache->is_stable)
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            const History history = cache->history[i];
            for(int32_t j = 0; j < history.count; j++)
            {
                const int32_t cycles_check = history.cycles[j];
                const int32_t parity_check = history.parity[j];
                if(MisMatch(cache, cycles_check, parity_check))
                {
                    cache->is_out_of_sync = true;
                    return;
                }
            }
        }
}

void Cache_ClearPacket(Cache* const cache)
{
    static Packet zero;
    cache->packet = zero;
    cache->turn++;
}

void Cache_ClearHistory(Cache* const cache)
{
    static History zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        cache->history[i] = zero;
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

int32_t Cache_GetCycleMin(Cache* const cache)
{
    int32_t min = INT32_MAX;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = cache->cycles[i];
        if(cycles != 0 && cycles < min)
            min = cycles;
    }
    return min;
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
    const int32_t kp = 2;
    const int32_t ki = 16;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = cache->cycles[i];
        if(cycles > 0)
        {
            const int32_t diff = setpoint - cycles;
            cache->integral[i] += diff;
            const int32_t ep = diff / kp;
            const int32_t ei = cache->integral[i] / ki;
            const int32_t control = ep + ei;
            printf("%d %d :: %d ... %d\n", ep, ei, control, cache->integral[i]);
            if(control >= 0)
                cache->control[i] = control;
        }
    }
}
