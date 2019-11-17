#include "Icon.h"

#include "Icons.h"
#include "Util.h"

#include <SDL2/SDL.h>

/* Hotkeys */

static const char hotkeys[] = {
    'Q', 'W', 'E', 'R', 'T', 'A', 'S', 'D', 'F', 'G', 'Z', 'X', 'C', 'V', 'B'
};

const char* Icon_GetHotkeys(void)
{
    return hotkeys;
}

int32_t Icon_GetHotkeysLen(void)
{
    return UTIL_LEN(hotkeys);
}

/* Buildings */

#define BUILD_AGE_1 ICON_BUILD_HOUSE, ICON_BUILD_MILL, ICON_BUILD_STONE_CAMP, ICON_BUILD_LUMBER_CAMP, ICON_BUILD_BARRACKS, ICON_BUILD_OUTPOST, ICON_BUILD_TOWN_CENTER
#define BUILD_AGE_2 ICON_NONE
#define BUILD_AGE_3 ICON_NONE
#define BUILD_AGE_4 ICON_NONE

static const Icon build_age1[] = { BUILD_AGE_1 };
static const Icon build_age2[] = { BUILD_AGE_1, BUILD_AGE_2 };
static const Icon build_age3[] = { BUILD_AGE_1, BUILD_AGE_2, BUILD_AGE_3 };
static const Icon build_age4[] = { BUILD_AGE_1, BUILD_AGE_2, BUILD_AGE_3, BUILD_AGE_4 };

const Icon* Icon_GetBuilding(const int32_t age)
{
    const Icon* ages[] = { build_age1, build_age2, build_age3, build_age4, };
    return ages[age];
}

int32_t Icon_GetBuildingLen(const int32_t age)
{
    const int32_t lens[] = { UTIL_LEN(build_age1), UTIL_LEN(build_age2), UTIL_LEN(build_age3), UTIL_LEN(build_age4) };
    return lens[age];
}

/* Barracks */

#define BARRACKS_AGE_1 ICON_UNIT_MILITIA
#define BARRACKS_AGE_2 ICON_NONE
#define BARRACKS_AGE_3 ICON_NONE
#define BARRACKS_AGE_4 ICON_NONE

static const Icon barracks_age1[] = { BARRACKS_AGE_1 };
static const Icon barracks_age2[] = { BARRACKS_AGE_1, BARRACKS_AGE_2 };
static const Icon barracks_age3[] = { BARRACKS_AGE_1, BARRACKS_AGE_2, BARRACKS_AGE_3 };
static const Icon barracks_age4[] = { BARRACKS_AGE_1, BARRACKS_AGE_2, BARRACKS_AGE_3, BARRACKS_AGE_4 };

const Icon* Icon_GetBarracks(const int32_t age)
{
    const Icon* ages[] = { barracks_age1, barracks_age2, barracks_age3, barracks_age4 };
    return ages[age];
}

int32_t Icon_GetBarracksLen(const int32_t age)
{
    const int32_t lens[] = { UTIL_LEN(barracks_age1), UTIL_LEN(barracks_age2), UTIL_LEN(barracks_age3), UTIL_LEN(barracks_age4) };
    return lens[age];
}

/* Town Center */

#define TOWN_CENTER_AGE_1 ICON_UNIT_MALE_VILLAGER
#define TOWN_CENTER_AGE_2 ICON_NONE
#define TOWN_CENTER_AGE_3 ICON_NONE
#define TOWN_CENTER_AGE_4 ICON_NONE

static const Icon town_center_age1[] = { TOWN_CENTER_AGE_1 };
static const Icon town_center_age2[] = { TOWN_CENTER_AGE_1, TOWN_CENTER_AGE_2 };
static const Icon town_center_age3[] = { TOWN_CENTER_AGE_1, TOWN_CENTER_AGE_2, TOWN_CENTER_AGE_3 };
static const Icon town_center_age4[] = { TOWN_CENTER_AGE_1, TOWN_CENTER_AGE_2, TOWN_CENTER_AGE_3, TOWN_CENTER_AGE_4 };

const Icon* Icon_GetTownCenter(const int32_t age)
{
    const Icon* ages[] = { town_center_age1, town_center_age2, town_center_age3, town_center_age4 };
    return ages[age];
}

int32_t Icon_GetTownCenterLen(const int32_t age)
{
    const int32_t lens[] = { UTIL_LEN(town_center_age1), UTIL_LEN(town_center_age2), UTIL_LEN(town_center_age3), UTIL_LEN(town_center_age4) };
    return lens[age];
}

Icon Icon_FromOverview(const Overview overview, const Motive motive)
{
    const int32_t age = 0; // XXX. SHOULD BE TOP LEVEL.
    const Icons icons = Icons_FromMotive(motive, age);
    int32_t index = -1;
    if(overview.key_q) index =  0;
    if(overview.key_w) index =  1;
    if(overview.key_e) index =  2;
    if(overview.key_r) index =  3;
    if(overview.key_t) index =  4;
    if(overview.key_a) index =  5;
    if(overview.key_s) index =  6;
    if(overview.key_d) index =  7;
    if(overview.key_f) index =  8;
    if(overview.key_g) index =  9;
    if(overview.key_z) index = 10;
    if(overview.key_x) index = 11;
    if(overview.key_c) index = 12;
    if(overview.key_v) index = 13;
    if(overview.key_b) index = 14;
    return (index != -1 && icons.icon != NULL && index < icons.count) ? icons.icon[index] : ICON_NONE;

}
