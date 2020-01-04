#pragma once

#include "Icon.h"
#include "Motive.h"
#include "Overview.h"
#include "Age.h"

typedef struct
{
    IconType icon_type;
    union
    {
        IconBuild icon_build;
        IconUnit icon_unit;
        IconTech icon_tech;
        int32_t index;
    }
    uni;
}
Button;

Button Button_FromOverview(const Overview);

const char* Button_GetHotkeys(void);

int32_t Button_GetHotkeysLen(void);
