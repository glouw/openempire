#pragma once

#include "Age.h"
#include "Part.h"
#include "Icon.h"
#include "Civ.h"

#include <stdint.h>

typedef struct
{
    Part* part;
    int32_t count;
}
Parts;

Parts Parts_GetMaleVillager(void);

Parts Parts_GetFemaleVillager(void);

Parts Parts_GetRedArrows(void);

Parts Parts_GetSmoke(void);

Parts Parts_GetFire(void);

Parts Parts_GetTownCenter(const Age age);

Parts Parts_FromIcon(const Icon, const Age, const Civ);

void Parts_Free(const Parts);
