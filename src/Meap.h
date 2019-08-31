#pragma once

#include "Point.h"

#include <stdint.h>

typedef struct
{
    Point point;
    int32_t prio;
}
Step;

typedef struct
{
    Step* step;
    int32_t size;
    int32_t max;
}
Meap;

Meap Meap_Init(void);

void Meap_Insert(Meap* const, const int32_t prio, const Point);

Step Meap_Delete(Meap* const);

void Meap_Free(Meap*);
