#pragma once

#include "Step.h"

typedef struct
{
    Step* step;
    int32_t size;
    int32_t max;
}
Meap;

Meap Meap_Make(void);

void Meap_Insert(Meap* const, const int32_t prio, const Point);

Step Meap_Delete(Meap* const);

void Meap_Free(Meap*);
