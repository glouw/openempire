#include "Icons.h"

#include "Util.h"

#include <stdlib.h>

/* Buildings */

#define BUILD_AGE_1 \
    ICON_BUILD_BARRACKS, \
    ICON_BUILD_MILL, \
    ICON_BUILD_TOWN_CENTER, \
    ICON_BUILD_HOUSE, \
    ICON_BUILD_OUTPOST, \
    ICON_BUILD_STONE_CAMP, \
    ICON_BUILD_LUMBER_CAMP
#define BUILD_AGE_2 ICON_NONE
#define BUILD_AGE_3 ICON_NONE
#define BUILD_AGE_4 ICON_NONE

static const Icon build_age1[] = { BUILD_AGE_1 };
static const Icon build_age2[] = { BUILD_AGE_1 };
static const Icon build_age3[] = { BUILD_AGE_1 };
static const Icon build_age4[] = { BUILD_AGE_1 };

static const Icon* GetBuilding(const Age age)
{
    const Icon* ages[] = {
        build_age1, build_age2, build_age3, build_age4
    };
    return ages[(int32_t) age];
}

static int32_t GetBuildingLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(build_age1), UTIL_LEN(build_age2), UTIL_LEN(build_age3), UTIL_LEN(build_age4)
    };
    return lens[(int32_t) age];
}

/* Barracks */

#define BARRACKS_AGE_1 ICON_UNIT_MILITIA
#define BARRACKS_AGE_2 ICON_NONE
#define BARRACKS_AGE_3 ICON_NONE
#define BARRACKS_AGE_4 ICON_NONE

static const Icon barracks_age1[] = { BARRACKS_AGE_1 };
static const Icon barracks_age2[] = { BARRACKS_AGE_1 };
static const Icon barracks_age3[] = { BARRACKS_AGE_1 };
static const Icon barracks_age4[] = { BARRACKS_AGE_1 };

static const Icon* GetBarracks(const Age age)
{
    const Icon* ages[] = {
        barracks_age1, barracks_age2, barracks_age3, barracks_age4
    };
    return ages[(int32_t) age];
}

static int32_t GetBarracksLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(barracks_age1), UTIL_LEN(barracks_age2), UTIL_LEN(barracks_age3), UTIL_LEN(barracks_age4)
    };
    return lens[(int32_t) age];
}

/* Town Center */

#define TOWN_CENTER_AGE_1 ICON_UNIT_MALE_VILLAGER, ICON_UNIT_FEMALE_VILLAGER
#define TOWN_CENTER_AGE_2 ICON_NONE
#define TOWN_CENTER_AGE_3 ICON_NONE
#define TOWN_CENTER_AGE_4 ICON_NONE

static const Icon towncenter_age1[] = { TOWN_CENTER_AGE_1 };
static const Icon towncenter_age2[] = { TOWN_CENTER_AGE_1 };
static const Icon towncenter_age3[] = { TOWN_CENTER_AGE_1 };
static const Icon towncenter_age4[] = { TOWN_CENTER_AGE_1 };

static const Icon* GetTownCenter(const Age age)
{
    const Icon* ages[] = {
        towncenter_age1, towncenter_age2, towncenter_age3, towncenter_age4
    };
    return ages[(int32_t) age];
}

static int32_t GetTownCenterLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(towncenter_age1), UTIL_LEN(towncenter_age2), UTIL_LEN(towncenter_age3), UTIL_LEN(towncenter_age4)
    };
    return lens[(int32_t) age];
}

Icons Icons_FromMotive(const Motive motive, const Age age)
{
    static Icons zero;
    Icons icons = zero;
    icons.icon = NULL;
    icons.count = 0;
    switch(motive.action)
    {
    case ACTION_BUILD:
        icons.icon = GetBuilding(age);
        icons.count = GetBuildingLen(age);
        break;
    case ACTION_UNIT_TECH:
        switch(motive.type)
        {
        case TYPE_BARRACKS:
            icons.icon = GetBarracks(age);
            icons.count = GetBarracksLen(age);
            break;
        case TYPE_TOWN_CENTER:
            icons.icon = GetTownCenter(age);
            icons.count = GetTownCenterLen(age);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return icons;
}

bool Icons_IsIndexValid(const Icons icons, const int32_t index)
{
    return index != -1
        && icons.icon != NULL
        && index < icons.count;
}
