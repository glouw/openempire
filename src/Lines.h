#pragma once

#include "Line.h"

#include <stdint.h>

typedef struct
{
    Line* line;
    int32_t count;
    int32_t max;
}
Lines;

Lines Lines_New(const int32_t max);

Lines Lines_Append(Lines, const Line);

void Lines_Free(const Lines);

void Lines_Sort(const Lines);

void Lines_Print(const Lines);
