#pragma once

#include <stdint.h>

void Timer_Update(const int64_t cycles);

int64_t Timer_GetCycles(void);

int64_t Timer_GetKeyFrames(void);

int64_t Timer_GetDeathFrames(void);
