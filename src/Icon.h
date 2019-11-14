#pragma once

#include "Motive.h"
#include "Overview.h"
#include "Input.h"

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

const char* Icon_GetHotkeys(void);
int32_t Icon_GetHotkeysLen(void);

const Icon* Icon_GetBuilding(const int32_t age);
int32_t Icon_GetBuildingLen(const int32_t age);

const Icon* Icon_GetBarracks(const int32_t age);
int32_t Icon_GetBarracksLen(const int32_t age);

const Icon* Icon_GetTownCenter(const int32_t age);
int32_t Icon_GetTownCenterLen(const int32_t age);

Icon Icon_FromOverview(const Overview, const Motive);

