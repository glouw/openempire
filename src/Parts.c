#include "Parts.h"

#include "Util.h"

Parts Parts_GetMaleVillager(void)
{
    static const Part part[] = {
        { {0,0}, FILE_MALE_VILLAGER_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetFemaleVillager(void)
{
    static const Part part[] = {
        { {0,0}, FILE_FEMALE_VILLAGER_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetRedArrows(void)
{
    static const Part part[] = {
        { {0,0}, FILE_RIGHT_CLICK_RED_ARROWS },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetSmoke(void)
{
    static const Part part[] = {
        { {0,0}, FILE_SMALLER_EXPLOSION_SMOKE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetFire(void)
{
    static const Part a[] = { { {0,0}, FILE_FIRE_SMALL_A  } };
    static const Part b[] = { { {0,0}, FILE_FIRE_SMALL_B  } };
    static const Part c[] = { { {0,0}, FILE_FIRE_SMALL_C  } };
    static const Part d[] = { { {0,0}, FILE_FIRE_MEDIUM_A } };
    static const Part e[] = { { {0,0}, FILE_FIRE_MEDIUM_B } };
    Parts parts = { NULL, 1 };
    switch(Util_Rand() % 5)
    {
        case 0: parts.part = a; break;
        case 1: parts.part = b; break;
        case 2: parts.part = c; break;
        case 3: parts.part = d; break;
        case 4: parts.part = e; break;
    }
    return parts;
}

Parts Parts_GetTownCenter(const Age age)
{
    static const Part age1[] = {
        { {-0,0}, FILE_AGE_1_TOWN_CENTER_TOP },
        { {-1,1}, FILE_AGE_1_TOWN_CENTER_SHADOW },
        { {-2,2}, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, FILE_AGE_1_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    };
    static const Part age2[] = {
        { {-0,0}, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    static const Part age3[] = {
        { {-0,0}, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    static const Part age4[] = {
        { {-0,0}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_SHADOW },
        { {-1,1}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_FLOORING },
        { {-2,2}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    };
    Parts parts = { NULL, 0 };
    switch(age)
    {
    case AGE_1: parts.part = age1; parts.count = UTIL_LEN(age1); break;
    case AGE_2: parts.part = age2; parts.count = UTIL_LEN(age2); break;
    case AGE_3: parts.part = age3; parts.count = UTIL_LEN(age3); break;
    case AGE_4: parts.part = age4; parts.count = UTIL_LEN(age4); break;
    }
    return parts;
}

static Parts GetBarracks(const Age age)
{
    static const Part age1[] = { { {0,0}, FILE_AGE_1_BARRACKS } };
    static const Part age2[] = { { {0,0}, FILE_AGE_2_NORTH_EUROPE_BARRACKS } };
    static const Part age3[] = { { {0,0}, FILE_AGE_3_NORTH_EUROPE_BARRACKS } };
    Parts parts = { NULL, 0 };
    switch(age)
    {
    case AGE_1: parts.part = age1; parts.count = UTIL_LEN(age1); break;
    case AGE_2: parts.part = age2; parts.count = UTIL_LEN(age2); break;
    case AGE_3:
    case AGE_4: parts.part = age3; parts.count = UTIL_LEN(age3); break;
    }
    return parts;
}

static Parts GetMill(const Age age)
{
    static const Part age1[] = {
        { {0,0}, FILE_AGE_1_MILL },
        { {0,0}, FILE_AGE_1_MILL_DONKEY },
    };
    static const Part age2[] = {
        { {0,0}, FILE_AGE_2_NORTH_EUROPE_MILL },
        { {0,0}, FILE_AGE_2_NORTH_EUROPE_MILL_ANIMATION },
        { {0,0}, FILE_AGE_2_NORTH_EUROPE_MILL_SHADOW },
    };
    static const Part age3[] = {
        { {0,0}, FILE_AGE_3_NORTH_EUROPE_MILL },
        { {0,0}, FILE_AGE_3_NORTH_EUROPE_MILL_ANIMATION },
        { {0,0}, FILE_AGE_3_NORTH_EUROPE_MILL_SHADOW },
    };
    Parts parts = { NULL, 0 };
    switch(age)
    {
        case AGE_1: parts.part = age1; parts.count = UTIL_LEN(age1); break;
        case AGE_2: parts.part = age2; parts.count = UTIL_LEN(age2); break;
        case AGE_3:
        case AGE_4: parts.part = age3; parts.count = UTIL_LEN(age3); break;
    }
    return parts;
}

static Parts GetHouse(const Age age)
{
    static const Part age1[] = { { {0,0}, FILE_AGE_1_HOUSE }, };
    static const Part age2[] = { { {0,0}, FILE_AGE_2_NORTH_EUROPE_HOUSE }, };
    static const Part age3[] = { { {0,0}, FILE_AGE_3_NORTH_EUROPE_HOUSE }, };
    Parts parts = { NULL, 0 };
    switch(age)
    {
        case AGE_1: parts.part = age1; parts.count = UTIL_LEN(age1); break;
        case AGE_2: parts.part = age2; parts.count = UTIL_LEN(age2); break;
        case AGE_3:
        case AGE_4: parts.part = age3; parts.count = UTIL_LEN(age3); break;
    }
    return parts;
}

static Parts GetOutpost(void)
{
    static const Part part[] = {
        { {0,0}, FILE_AGE_1_OUTPOST },
        { {0,0}, FILE_AGE_1_OUTPOST_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetStoneCamp(void)
{
    static const Part part[] = {
        { {0,0}, FILE_NORTH_EUROPE_STONE_MINING_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetLumberCamp(void)
{
    static const Part part[] = {
        { {0,0}, FILE_NORTH_EUROPE_LUMBER_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetMilitia(void)
{
    static const Part part[] = {
        { {0,0}, FILE_MILITIA_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_FromIcon(const Icon icon)
{
    const Age age = AGE_4;
    static Parts none;
    switch(icon)
    {
        // DO NOT USE DEFAULT - COMPILER WILL WARN OF MISSING CASES.
        case ICON_BUILD_TOWN_CENTER    : return Parts_GetTownCenter(age);
        case ICON_BUILD_BARRACKS       : return GetBarracks(age);
        case ICON_BUILD_MILL           : return GetMill(age);
        case ICON_BUILD_HOUSE          : return GetHouse(age);
        case ICON_BUILD_OUTPOST        : return GetOutpost();
        case ICON_BUILD_STONE_CAMP     : return GetStoneCamp();
        case ICON_BUILD_LUMBER_CAMP    : return GetLumberCamp();
        case ICON_UNIT_MILITIA         : return GetMilitia();
        case ICON_UNIT_MALE_VILLAGER   : return Parts_GetMaleVillager();
        case ICON_UNIT_FEMALE_VILLAGER : return Parts_GetFemaleVillager();
        case ICON_NONE:
            return none;
    }
    return none;
}
