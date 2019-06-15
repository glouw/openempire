#pragma once

#include <stdint.h>

typedef struct
{
    int64_t cycles;
    int64_t key_frames;
}
Timer;

void Timer_Update(const int64_t cycles);

int64_t Timer_GetCycles(void);

int64_t Timer_GetKeyFrames(void);
