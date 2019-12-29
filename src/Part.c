#include "Part.h"

bool Part_MustSubtractIndex(Part* const part, const Civ civ)
{
    return part->subtract_index && civ > CIV_NORTH_EUROPE;
}
