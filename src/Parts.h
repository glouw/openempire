#pragma once

#include "Part.h"
#include "Icon.h"

#include <stdint.h>

typedef struct
{
    const Part* part;
    int32_t count;
}
Parts;

Parts Parts_GetVillager(void);

Parts Parts_GetRedArrows(void);

Parts Parts_GetSmoke(void);

Parts Parts_GetFire(void);

Parts Parts_GetTownCenterAge1(void);

Parts Parts_GetTownCenterAge2(void);

Parts Parts_GetTownCenterAge3(void);

Parts Parts_GetTownCenterAge4(void);

Parts Parts_FromIcon(const Icon);
