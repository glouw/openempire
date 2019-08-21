#pragma once

#include "Point.h"

#include <stdint.h>

typedef struct
{
    int32_t prio;
    Point point;
}
Step;

typedef struct
{
    int32_t size;
    int32_t max;
    Step* step;
}
Meap;

Meap Meap_Init(void);

void Meap_Insert(Meap* const, const int32_t prio, const Point);

Step Meap_Delete(Meap* const);

void Meap_Free(Meap*);
