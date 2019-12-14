#include "Parts.h"

#include "Util.h"

Parts Parts_GetVillager(void)
{
    static const Part part[] = {
        { {0,0}, false, FILE_MALE_VILLAGER_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetRedArrows(void)
{
    static const Part part[] = {
        { {0,0}, false, FILE_RIGHT_CLICK_RED_ARROWS },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetSmoke(void)
{
    static const Part part[] = {
        { {0,0}, false, FILE_SMALLER_EXPLOSION_SMOKE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetFire(void)
{
    static const Part part[] = {
        { {0,0}, false, FILE_FIRE_SMALL_A },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetTownCenterAge1(void)
{
    static const Part part[] = {
        { {-0,0}, true,  FILE_AGE_1_TOWN_CENTER_TOP },
        { {-1,1}, true,  FILE_AGE_1_TOWN_CENTER_SHADOW },
        { {-3,2}, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-3,2}, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetTownCenterAge2(void)
{
    static const Part part[] = {
        { {-0,0}, true, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, true, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetTownCenterAge3(void)
{
    static const Part part[] = {
        { {-0,0}, true, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, true, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetTownCenterAge4(void)
{
    static const Part part[] = {
        { {-0,0}, true, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, true, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetBarracks(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_AGE_1_BARRACKS },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetMill(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_AGE_1_MILL },
        { {0,0}, true, FILE_AGE_1_MILL_DONKEY },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetHouse(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_AGE_1_HOUSE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetOutpost(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_AGE_1_OUTPOST },
        { {0,0}, true, FILE_AGE_1_OUTPOST_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetStoneCamp(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_NORTH_EUROPE_STONE_MINING_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetLumberCamp(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_NORTH_EUROPE_LUMBER_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetMilitia(void)
{
    static const Part part[] = {
        { {0,0}, true, FILE_MILITIA_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_FromIcon(const Icon icon)
{
    static Parts none;
    switch(icon)
    {
        // DO NOT USE DEFAULt - COMPILER WILL WARN OF MISSING CASES.
        case ICON_BUILD_TOWN_CENTER  : return Parts_GetTownCenterAge1();
        case ICON_BUILD_BARRACKS     : return GetBarracks();
        case ICON_BUILD_MILL         : return GetMill();
        case ICON_BUILD_HOUSE        : return GetHouse();
        case ICON_BUILD_OUTPOST      : return GetOutpost();
        case ICON_BUILD_STONE_CAMP   : return GetStoneCamp();
        case ICON_BUILD_LUMBER_CAMP  : return GetLumberCamp();
        case ICON_UNIT_MILITIA       : return GetMilitia();
        case ICON_UNIT_MALE_VILLAGER : return Parts_GetVillager();
        case ICON_NONE:
            return none;
    }
    return none;
}
