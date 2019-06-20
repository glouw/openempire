#include "Timer.h"

typedef struct
{
    int64_t cycles;
    int64_t key_frames;
    int64_t death_frames;
}
Timer;

static Timer timer;

void Timer_Update(const int64_t cycles)
{
    timer.cycles = cycles;
    timer.key_frames = cycles / 3;
    timer.death_frames = cycles / 3000;
}

int64_t Timer_GetCycles(void)
{
    return timer.cycles;
}

int64_t Timer_GetKeyFrames(void)
{
    return timer.key_frames;
}

int64_t Timer_GetDeathFrames(void)
{
    return timer.death_frames;
}
