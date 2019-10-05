#include "Window.h"

#include "Config.h"

Window Window_Make(const Overview overview)
{
    const Quad units = Overview_GetRenderBox(overview, CONFIG_VIDEO_TOP_LEFT_BORDER_OFFSET_UNITS);
    const Quad terrain = Overview_GetRenderBox(overview, CONFIG_VIDEO_TOP_LEFT_BORDER_OFFSET_TERRAIN);
    const Window window = {
        Quad_GetRenderPoints(units),
        Quad_GetRenderPoints(terrain),
    };
    return window;
}

void Window_Free(const Window window)
{
    Points_Free(window.units);
    Points_Free(window.terrain);
}
