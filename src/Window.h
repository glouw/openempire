#pragma once

#include "Points.h"
#include "Overview.h"

typedef struct
{
    Points units;
    Points terrain;
}
Window;

Window Window_Make(const Overview, const Grid);

void Window_Free(const Window);
