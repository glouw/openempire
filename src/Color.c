#include "Color.h"

uint32_t Color_ToRgb(const Color color)
{
    switch(color)
    {
#define COLOR_X(name, hex) case name: return hex;
        COLOR_X_MACRO
#undef COLOR_X
    case COLOR_COUNT:
        break;
    }
    return 0x00000000;
}
