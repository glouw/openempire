#pragma once

#include <stdint.h>

typedef enum
{
    // Building Icons.
    ICON_BUILD_BARRACKS = 2,
    ICON_BUILD_MILL = 19,
    ICON_BUILD_PALISADE_WALL = 30,
    ICON_BUILD_HOUSE = 34,
    ICON_BUILD_OUTPOST = 38,
    ICON_BUILD_STONE_CAMP = 39,
    ICON_BUILD_LUMBER_CAMP = 40,

    // Unit Icons.
    ICON_UNIT_MILITIA = 8,
    ICON_UNIT_VILLAGER = 15,
}
Icon;

const Icon* Icon_GetAge1(void);

int32_t Icon_GetAge1Len(void);
