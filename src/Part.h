#pragma once

#include "Point.h"
#include "Graphics.h"

#include <stdbool.h>

typedef struct
{
    Point cart;
    bool at_center;
    Graphics file;
}
Part;
