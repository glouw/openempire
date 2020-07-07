#include "Cache.h"

#include "Util.h"
#include "Config.h"

Cache Cache_Make(const int32_t users)
{
    static Cache zero;
    Cache cache = zero;
    cache.users = users;
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

void Cache_CheckParity(Cache* const cache)
{
    if(cache->is_stable)
    {
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
                    puts("OUT OF SYNC DETECTED");
                    return;
                }
            }
        }
        cache->is_out_of_sync = false;
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
    {
        cache->history[i] = zero;
        cache->integral[i] = 0;
    }
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
    const int32_t max =  INT32_MAX;
    int32_t min = max;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = cache->cycles[i];
        if(cycles != 0 && cycles < min)
            min = cycles;
    }
    return min == max ? 0 : min;
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

static bool IsActiveColor(Cache* const cache, const Color color)
{
    return cache->cycles[color] > 0;
}

static bool IsUnderSaturated(Cache* const cache)
{
    bool under = true;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        if(IsActiveColor(cache, i))
            under &= cache->control[i] < 0;
    return under;
}

static bool IsOverSaturated(Cache* const cache)
{
    bool over = true;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        if(IsActiveColor(cache, i))
            over &= cache->control[i] > 0;
    return over;
}

static bool IsSaturated(Cache* const cache)
{
    return IsUnderSaturated(cache) || IsOverSaturated(cache);
}

static void ResetControl(Cache* const cache)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        cache->control[i] = 0;
        cache->integral[i] = 0;
    }
}

void Cache_CalculateControl(Cache* const cache, const int32_t setpoint)
{
    // HIGHER KP, KI VALUES LOWER THE PI DRIVE STRENGTH
    const int32_t kp =  32;
    const int32_t ki =  64;
    const int32_t si = 512; // XXX. INTEGRAL SATURATE... ARBITRARY?
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        if(IsActiveColor(cache, i))
        {
            const int32_t diff = setpoint - cache->cycles[i];
            cache->integral[i] += diff;
            if(cache->integral[i] < -si) cache->integral[i] = -si;
            if(cache->integral[i] > +si) cache->integral[i] = +si;
            const int32_t ep = diff;
            const int32_t ei = cache->integral[i];
            const int32_t control = (ki * ep + kp * ei) / (kp * ki);
            cache->control[i] = control;
        }
    }
    if(IsSaturated(cache))
        ResetControl(cache);
}
