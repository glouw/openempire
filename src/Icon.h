#pragma once

#include "Motive.h"
#include "Overview.h"

#include <stdint.h>

typedef enum
{
    // Building Icons.
    ICON_NONE = -1,
    ICON_BUILD_BARRACKS = 2,
    ICON_BUILD_MILL = 19,
    ICON_BUILD_TOWN_CENTER = 28,
    ICON_BUILD_HOUSE = 34,
    ICON_BUILD_OUTPOST = 38,
    ICON_BUILD_STONE_CAMP = 39,
    ICON_BUILD_LUMBER_CAMP = 40,

    // Unit Icons.
    ICON_UNIT_MILITIA = 8,
    ICON_UNIT_MALE_VILLAGER = 15,
    ICON_UNIT_FEMALE_VILLAGER = 16,
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

Icon Icon_FromOverview(const Overview, const Motive);

const char* Icon_GetHotkeys(void);

int32_t Icon_GetHotkeysLen(void);
