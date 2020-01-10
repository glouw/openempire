#pragma once

#include "Traffic.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    Traffic* traffic;
    int32_t a;
    int32_t b;
    int32_t max;
}
Traffics;

Traffics Traffics_Init(const int max);

Traffics Traffics_Queue(Traffics, Traffic);

void Traffics_Free(const Traffics);

int32_t Traffics_Size(const Traffics);

bool Traffics_Active(const Traffics);
