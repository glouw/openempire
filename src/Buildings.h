#pragma once

#include "Building.h"
#include "Grid.h"

#include <stdint.h>

typedef struct
{
    Building* build;
    Building** reference;
    int32_t count;
    int32_t max;
    int32_t rows;
    int32_t cols;
}
Buildings;

Buildings Buildings_New(const Grid);

Buildings Buildings_Append(Buildings, const Building);

void Buildings_Free(const Buildings);
