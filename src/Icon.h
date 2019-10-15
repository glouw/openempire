#pragma once

#define ICON_AGE_COUNT (4)

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

#define ICON_AGE_1 \
    ICON_BUILD_BARRACKS,\
    ICON_BUILD_MILL,\
    ICON_BUILD_PALISADE_WALL,\
    ICON_BUILD_HOUSE,\
    ICON_BUILD_OUTPOST,\
    ICON_BUILD_STONE_CAMP,\
    ICON_BUILD_LUMBER_CAMP,\

static const Icon Icon_Age1[] = { ICON_AGE_1 };
static const Icon Icon_Age2[] = { ICON_AGE_1 };
static const Icon Icon_Age3[] = { ICON_AGE_1 };
static const Icon Icon_Age4[] = { ICON_AGE_1 };
