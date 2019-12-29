#pragma once

#include "Point.h"
#include "Graphics.h"

#include <stdbool.h>

typedef struct
{
    Point cart;
    bool can_civ;
    Graphics file;
}
Part;
