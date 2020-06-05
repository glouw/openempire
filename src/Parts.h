#pragma once

#include "Age.h"
#include "Part.h"
#include "Button.h"

#include <stdint.h>

typedef struct
{
    Part* part;
    int32_t count;
}
Parts;

Parts Parts_FromButton(const Button, const Age);

void Parts_Free(const Parts);

// THESE PARTS HAVE TO BE EXPORTED AS THEY ARE NOT CREATED THROUGH A BUTTON.

Parts Parts_GetWayPointFlag(void);

Parts Parts_GetRedArrows(void);

Parts Parts_GetSmoke(void);

Parts Parts_GetFire(void);

Parts Parts_GetForestTree(void);

Parts Parts_GetStoneMine(void);

Parts Parts_GetGoldMine(void);

Parts Parts_GetBerryBush(void);
