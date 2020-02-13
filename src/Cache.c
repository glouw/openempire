#include "Cache.h"

#include "Util.h"

Cache Cache_Init(const int32_t users, const int32_t map_power)
{
    static Cache zero;
    Cache cache = zero;
    cache.users = users;
    cache.map_power = map_power;
    cache.seed = rand();
    return cache;
}

void Cache_AppendParity(Cache* cache, const Color color, const Parity parity)
{
    int32_t i = 0;
    while(true)
    {
        const Parity avail = cache->parity[color][i];
        if(Parity_IsZero(avail))
            break;
        i++;
    }
    cache->parity[color][i] = parity;
}

void Cache_CalcOutOfSync(Cache* const cache)
{
    cache->is_out_of_sync = false;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
        for(int32_t x = 0; x < COLOR_COUNT; x++)
        for(int32_t y = 0; y < CONFIG_MAIN_LOOP_SPEED_FPS; y++)
        {
            const Parity a = cache->parity[x][y];
            const Parity b = cache->parity[i][j];
            if(a.cycles == b.cycles
            && a.xorred != b.xorred)
                cache->is_out_of_sync = true;
        }
}

void Cache_Clear(Cache* const cache)
{
    static Packet zero;
    static Parity none;
    cache->packet = zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
        cache->parity[i][j] = none;
    cache->turn++;
}

int32_t Cache_GetCycleSetpoint(Cache* const cache)
{
    int32_t setpoint = 0;
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const int32_t cycles = cache->parity[i][j].cycles;
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
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const int32_t cycles = cache->parity[i][j].cycles;
        if(cycles > max)
            max = cycles;
    }
    return max;
}

static int32_t GetCycleAverage(Cache* const cache)
{
    int32_t count = 0;
    int32_t sum = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const Parity parity = cache->parity[i][j];
        if(!Parity_IsZero(parity))
        {
            sum += parity.cycles;
            count++;
        }
    }
    return count == 0 ? 0 : (sum / count);
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
    {
        const int32_t average = GetCycleAverage(cache);
        cache->control[i] = (average < setpoint)
            ? PACKET_CONTROL_SPEED_UP
            : PACKET_CONTROL_STEADY;
    }
}

void Cache_CalcStability(Cache* const cache, const int32_t setpoint)
{
    cache->is_stable = setpoint > CONFIG_SOCKETS_THRESHOLD_START;
}

void Cache_DumpPanicTable(Cache* const cache)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        Parity* const panic = cache->panic[i];
        for(int32_t j = 0; j < BACKUP_MAX; j++)
            printf("0x%016lX %5d\n", panic[j].xorred, panic[j].cycles);
        putchar('\n');
    }
}

static int32_t GetIndex(Parity* const parities, const Parity parity)
{
    for(int32_t i = 0; i < BACKUP_MAX; i++)
        if(Parity_IsEqual(parities[i], parity))
            return i;
    return -1;
}

void Cache_FindPanicSolution(Cache* const cache, int32_t indices[COLOR_COUNT])
{
    int32_t solutions = 1;
    for(int32_t i = 0; i < BACKUP_MAX; i++)
    {
        const Parity parity = cache->panic[COLOR_BLU][i];
        for(int32_t j = 1; j < COLOR_COUNT; j++)
        {
            Parity* const parities = cache->panic[j];
            const int32_t index = GetIndex(parities, parity);
            if(index != -1)
            {
                indices[j] = index;
                solutions++;
            }
        }
        indices[COLOR_BLU] = i;
        if(solutions == cache->users_connected)
        {
            puts("SOLUTION FOUND\n");
            return;
        }
    }
    Util_Bomb("NO SOLUTION FOUND - CLIENTS TOTALLY OUT OF SYNC\n");
}
