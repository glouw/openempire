#include "Color.h"

static Color my_color;

void Color_SetMyColor(const Color color)
{
    my_color = color;
}

Color Color_GetMyColor(void)
{
    return my_color;
}
