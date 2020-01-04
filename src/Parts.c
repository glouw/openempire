// * SHADOWS MUST BE DEFINED LAST FOR PARTS SUCH THAT SHADOWS MAY BE TOGGLED BY SUBTRACTING 1 FROM PART COUNT.
// * PARTS MUST ONLY BE DEFINED AS NORTH EUROPEAN; CIV OFFSETS (+1, +2, +3) CHANGE BUILDING SKINS TO ASIA, ARAB, AND WEST EUROPE, RESPECTIVELY.
// * SOME PART COLLECTIONS, LIKE THE ASIAN MILL, DO NOT PATTERN MATCH OTHER PART COLLECTIONS (EG. NO SHADOW UNLIKE OTHER MILLS) AND REQUIRE SPECIAL CARE.

#include "Parts.h"

#include "Util.h"

static Parts GetAgeUpFlag(void)
{
    static Part part[] = { { {0,0}, TRIGGER_AGE_UP, false, false, FILE_FLAG_TALL } };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetMaleVillager(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_MALE_VILLAGER_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetFemaleVillager(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_FEMALE_VILLAGER_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetRedArrows(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_RIGHT_CLICK_RED_ARROWS },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetSmoke(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_SMALLER_EXPLOSION_SMOKE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

Parts Parts_GetFire(void)
{
    static Part a[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_FIRE_SMALL_A  } };
    static Part b[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_FIRE_SMALL_B  } };
    static Part c[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_FIRE_SMALL_C  } };
    static Part d[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_FIRE_MEDIUM_A } };
    static Part e[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_FIRE_MEDIUM_B } };
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

static Parts GetTownCenter(const Age age)
{
    static Part age1[] = {
        { {-0,0}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_TOP },
        { {-2,2}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
        { {-1,1}, TRIGGER_NONE, false, false, FILE_AGE_1_TOWN_CENTER_SHADOW },
    };
    static Part age2[] = {
        { {-0,0}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    static Part age3[] = {
        { {-0,0}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_TOWN_CENTER_SHADOW },
    };
    static Part age4[] = {
        { {-0,0}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_TOP },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_FLOORING },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE },
        { {-2,2}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
        { {-1,1}, TRIGGER_NONE, true, false, FILE_AGE_4_NORTH_EUROPE_TOWN_CENTER_SHADOW },
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
    static Part age1[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_AGE_1_BARRACKS } };
    static Part age2[] = { { {0,0}, TRIGGER_NONE, true,  false, FILE_AGE_2_NORTH_EUROPE_BARRACKS } };
    static Part age3[] = { { {0,0}, TRIGGER_NONE, true,  false, FILE_AGE_3_NORTH_EUROPE_BARRACKS } };
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

static Parts DropAsianMillShadow(Parts parts, const Civ civ, const Age age)
{
    if(civ == CIV_ASIA)
        if(age == AGE_2
        || age == AGE_3
        || age == AGE_4)
            parts.count -= 1;
    return parts;
}

static Parts GetMill(const Age age, const Civ civ)
{
    static Part age1[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_AGE_1_MILL },
        { {0,0}, TRIGGER_NONE, false, false, FILE_AGE_1_MILL_DONKEY },
    };
    static Part age2[] = {
        { {0,0}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_MILL },
        { {0,0}, TRIGGER_NONE, true, false, FILE_AGE_2_NORTH_EUROPE_MILL_ANIMATION },
        { {0,0}, TRIGGER_NONE, true, true,  FILE_AGE_2_NORTH_EUROPE_MILL_SHADOW },
    };
    static Part age3[] = {
        { {0,0}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_MILL },
        { {0,0}, TRIGGER_NONE, true, false, FILE_AGE_3_NORTH_EUROPE_MILL_ANIMATION },
        { {0,0}, TRIGGER_NONE, true, true,  FILE_AGE_3_NORTH_EUROPE_MILL_SHADOW },
    };
    Parts parts = { NULL, 0 };
    switch(age)
    {
        case AGE_1: parts.part = age1; parts.count = UTIL_LEN(age1); break;
        case AGE_2: parts.part = age2; parts.count = UTIL_LEN(age2); break;
        case AGE_3:
        case AGE_4: parts.part = age3; parts.count = UTIL_LEN(age3); break;
    }
    return DropAsianMillShadow(parts, civ, age);
}

static Parts GetHouse(const Age age)
{
    static Part age1[] = { { {0,0}, TRIGGER_NONE, false, false, FILE_AGE_1_HOUSE } };
    static Part age2[] = { { {0,0}, TRIGGER_NONE, true,  false, FILE_AGE_2_NORTH_EUROPE_HOUSE } };
    static Part age3[] = { { {0,0}, TRIGGER_NONE, true,  false, FILE_AGE_3_NORTH_EUROPE_HOUSE } };
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
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_OUTPOST },
        { {0,0}, TRIGGER_NONE, false, false, FILE_OUTPOST_SHADOW },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetStoneCamp(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, true, false, FILE_NORTH_EUROPE_STONE_MINING_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetLumberCamp(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, true, false, FILE_NORTH_EUROPE_LUMBER_CAMP },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts GetMilitia(void)
{
    static Part part[] = {
        { {0,0}, TRIGGER_NONE, false, false, FILE_MILITIA_IDLE },
    };
    const Parts parts = { part, UTIL_LEN(part) };
    return parts;
}

static Parts Copy(const Parts parts)
{
    Parts copy;
    copy.count = parts.count;
    copy.part = UTIL_ALLOC(Part, copy.count);
    for(int32_t i = 0; i < copy.count; i++)
        copy.part[i] = parts.part[i];
    return copy;
}

static void SetCiv(const Parts parts, const Civ civ)
{
    for(int32_t i = 0; i < parts.count; i++)
    {
        Part* const part = &parts.part[i];
        if(part->can_civ)
        {
            const int32_t offset = Part_MustSubtractIndex(part, civ)
                ? civ - 1
                : civ;
            part->file = (Graphics) (part->file + offset);
        }
    }
}

static Parts Lookup(const Button button, const Age age, const Civ civ)
{
    switch(button.icon_type)
    {
    case ICONTYPE_BUILD:
        switch(button.uni.icon_build)
        {
        case ICONBUILD_BARRACKS    : return GetBarracks  (age     );
        case ICONBUILD_MILL        : return GetMill      (age, civ);
        case ICONBUILD_HOUSE       : return GetHouse     (age     );
        case ICONBUILD_OUTPOST     : return GetOutpost   (        );
        case ICONBUILD_STONE_CAMP  : return GetStoneCamp (        );
        case ICONBUILD_LUMBER_CAMP : return GetLumberCamp(        );
        case ICONBUILD_TOWN_CENTER : return GetTownCenter(age     );
        }
        break;
    case ICONTYPE_TECH:
        switch(button.uni.icon_tech)
        {
        case ICONTECH_AGE_2 : return GetAgeUpFlag();
        case ICONTECH_AGE_3 : return GetAgeUpFlag();
        case ICONTECH_AGE_4 : return GetAgeUpFlag();
        }
        break;
    case ICONTYPE_UNIT:
        switch(button.uni.icon_unit)
        {
        case ICONUNIT_MILITIA         : return GetMilitia       ();
        case ICONUNIT_MALE_VILLAGER   : return GetMaleVillager  ();
        case ICONUNIT_FEMALE_VILLAGER : return GetFemaleVillager();
        }
        break;
    case ICONTYPE_NONE:
    case ICONTYPE_COUNT:
        break;
    }
    static Parts zero;
    return zero;
}

Parts Parts_FromButton(const Button button, const Age age, const Civ civ)
{
    const Parts copy = Copy(Lookup(button, age, civ));
    SetCiv(copy, civ);
    return copy;
}

void Parts_Free(const Parts parts)
{
    free(parts.part);
}
