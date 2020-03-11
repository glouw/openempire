#pragma once

#include "Motive.h"
#include "Overview.h"
#include "Age.h"

#include <stdint.h>

typedef enum
{
    ICONBUILD_BARRACKS = 2,
    ICONBUILD_MILL = 19,
    ICONBUILD_TOWN_CENTER = 28,
    ICONBUILD_HOUSE = 34,
    ICONBUILD_OUTPOST = 38,
    ICONBUILD_STONE_CAMP = 39,
    ICONBUILD_LUMBER_CAMP = 40,
    ICONBUILD_CASTLE = 7,
}
IconBuild;

typedef enum
{
    ICONTECH_AGE_2 = 30,
    ICONTECH_AGE_3 = 31,
    ICONTECH_RESEARCH_MAN_AT_ARMS = 85,
    ICONTECH_RESEARCH_LONG_SWORDSMAN = 48,
}
IconTech;

typedef enum
{
    ICONUNIT_MILITIA = 8,
    ICONUNIT_MAN_AT_ARMS = 10,
    ICONUNIT_LONG_SWORDSMAN = 13,
    ICONUNIT_MALE_VILLAGER = 15,
    ICONUNIT_FEMALE_VILLAGER = 16,
}
IconUnit;

typedef enum
{
    ICONCOMMAND_ATTACK_MOVE = 6,
}
IconCommand;

typedef enum
{
    ICONTYPE_NONE,
    ICONTYPE_BUILD,
    ICONTYPE_TECH,
    ICONTYPE_UNIT,
    ICONTYPE_COMMAND,
    ICONTYPE_COUNT,
}
IconType;
