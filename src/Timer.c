#include "Timer.h"

static Timer timer;

void Timer_Update(const int64_t cycles)
{
    timer.cycles = cycles;
    timer.key_frames = cycles / 3;
}

int64_t Timer_GetCycles(void)
{
    return timer.cycles;
}

int64_t Timer_GetKeyFrames(void)
{
    return timer.key_frames;
}
