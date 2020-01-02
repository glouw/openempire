#pragma once

#include "Age.h"
#include "Part.h"
#include "Civ.h"
#include "Button.h"

#include <stdint.h>

typedef struct
{
    Part* part;
    int32_t count;
}
Parts;

Parts Parts_GetRedArrows(void);

Parts Parts_GetSmoke(void);

Parts Parts_GetFire(void);

Parts Parts_FromButton(const Button, const Age, const Civ);

void Parts_Free(const Parts);
