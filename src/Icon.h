#pragma once

#include "Motive.h"
#include "Overview.h"
#include "Age.h"

#include <stdint.h>

typedef enum
{
    ICON_NONE = -1,
    ICON_BUILD_BARRACKS = 2,
    ICON_BUILD_MILL = 19,
    ICON_BUILD_TOWN_CENTER = 28,
    ICON_BUILD_HOUSE = 34,
    ICON_BUILD_OUTPOST = 38,
    ICON_BUILD_STONE_CAMP = 39,
    ICON_BUILD_LUMBER_CAMP = 40,

    ICON_UNIT_MILITIA = 8,
    ICON_UNIT_MALE_VILLAGER = 15,
    ICON_UNIT_FEMALE_VILLAGER = 16,

    ICON_TECH_AGE_2 = 30,
    ICON_TECH_AGE_3 = 31,
    ICON_TECH_AGE_4 = 32,
}
Icon;

typedef enum
{
    ICONTYPE_BUILDING,
    ICONTYPE_UNIT,
    ICONTYPE_TECH,
    ICONTYPE_COUNT,
}
IconType;

Icon Icon_FromOverview(const Overview, const Motive, const Age);

const char* Icon_GetHotkeys(void);

int32_t Icon_GetHotkeysLen(void);
