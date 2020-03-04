#pragma once

#include "Icon.h"
#include "Motive.h"
#include "Bits.h"
#include "Overview.h"
#include "Trigger.h"

typedef struct
{
    IconType icon_type;
    union
    {
        int32_t index;
        IconBuild icon_build;
        IconUnit icon_unit;
        IconTech icon_tech;
        IconCommand icon_command;
    };
    Trigger trigger;
}
Button;

Button Button_FromOverview(const Overview);

const char* Button_GetHotkeys(void);

int32_t Button_GetHotkeysLen(void);

Button Button_Upgrade(Button, const Bits);

bool Button_UseAttackMove(const Button);
