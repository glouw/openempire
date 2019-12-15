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

Parts Parts_GetMaleVillager(void);

Parts Parts_GetFemaleVillager(void);

Parts Parts_GetRedArrows(void);

Parts Parts_GetSmoke(void);

Parts Parts_GetFire(void);

Parts Parts_GetTownCenter(const int32_t age);

Parts Parts_FromIcon(const Icon);
