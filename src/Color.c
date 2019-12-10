#include "Color.h"

const char* Color_ToString(const Color color)
{
    switch(color)
    {
    case COLOR_BLU: return "Blue";
    case COLOR_RED: return "Red";
    case COLOR_GRN: return "Green";
    case COLOR_YEL: return "Yellow";
    case COLOR_ORG: return "Orange";
    case COLOR_CYN: return "Cyan";
    case COLOR_PRP: return "Purple";
    case COLOR_GRY: return "Grey";
    case COLOR_GAIA: return "Gaia";
    default:
        return "N/A";
    }
}
