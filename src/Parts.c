// * SHADOWS MUST BE DEFINED LAST FOR PARTS SUCH THAT SHADOWS MAY BE TOGGLED BY SUBTRACTING 1 FROM PART COUNT.

#include "Parts.h"

#include "Util.h"

static Part flag[] = {
    { {0,0}, FILE_GRAPHICS_FLAG_TALL }
};
static Part villager_male[] = {
    { {0,0}, FILE_GRAPHICS_MALE_VILLAGER_IDLE }
};
static Part villager_female[] = {
    { {0,0}, FILE_GRAPHICS_FEMALE_VILLAGER_IDLE }
};
static Part red_arrows[] = {
    { {0,0}, FILE_GRAPHICS_RIGHT_CLICK_RED_ARROWS }
};
static Part smoke_a[] = {
    { {0,0}, FILE_GRAPHICS_SMALLER_EXPLOSION_SMOKE }
};
static Part fire_a[] = { { {0,0}, FILE_GRAPHICS_FIRE_SMALL_A  } };
static Part fire_b[] = { { {0,0}, FILE_GRAPHICS_FIRE_SMALL_B  } };
static Part fire_c[] = { { {0,0}, FILE_GRAPHICS_FIRE_SMALL_C  } };
static Part fire_d[] = { { {0,0}, FILE_GRAPHICS_FIRE_MEDIUM_A } };
static Part fire_e[] = { { {0,0}, FILE_GRAPHICS_FIRE_MEDIUM_B } };

static Part town_center_age1[] = {
    { {-0,0}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_TOP },
    { {-2,2}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_LEFT },
    { {-2,2}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
    { {-2,2}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_RITE },
    { {-2,2}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    { {-1,1}, FILE_GRAPHICS_AGE_1_TOWN_CENTER_SHADOW },
};
static Part town_center_age2[] = {
    { {-0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_TOP },
    { {-2,2}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT },
    { {-2,2}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
    { {-2,2}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_RITE },
    { {-2,2}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    { {-1,1}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_TOWN_CENTER_SHADOW },
};
static Part town_center_age3[] = {
    { {-0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_TOP },
    { {-2,2}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT },
    { {-2,2}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
    { {-2,2}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_RITE },
    { {-2,2}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
    { {-1,1}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
    { {-1,1}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_TOWN_CENTER_SHADOW },
};
static Part barracks_age1[] = { { {0,0}, FILE_GRAPHICS_AGE_1_BARRACKS } };
static Part barracks_age2[] = { { {0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_BARRACKS } };
static Part barracks_age3[] = { { {0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_BARRACKS } };
static Part mill_age1[] = {
    { {0,0}, FILE_GRAPHICS_AGE_1_MILL },
    { {0,0}, FILE_GRAPHICS_AGE_1_MILL_DONKEY },
};
static Part mill_age2[] = {
    { {0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_MILL },
    { {0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_MILL_ANIMATION },
    { {0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_MILL_SHADOW },
};
static Part mill_age3[] = {
    { {0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_MILL },
    { {0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_MILL_ANIMATION },
    { {0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_MILL_SHADOW },
};
static Part house_age1[] = { { {0,0}, FILE_GRAPHICS_AGE_1_HOUSE } };
static Part house_age2[] = { { {0,0}, FILE_GRAPHICS_AGE_2_WEST_EUROPE_HOUSE } };
static Part house_age3[] = { { {0,0}, FILE_GRAPHICS_AGE_3_WEST_EUROPE_HOUSE } };
static Part outpost[] = {
    { {0,0}, FILE_GRAPHICS_OUTPOST },
    { {0,0}, FILE_GRAPHICS_OUTPOST_SHADOW },
};
static Part stone_camp[] = {
    { {0,0}, FILE_GRAPHICS_WEST_EUROPE_STONE_MINING_CAMP },
};
static Part lumber_camp[] = {
    { {0,0}, FILE_GRAPHICS_WEST_EUROPE_LUMBER_CAMP },
};
static Part castle[] = {
    { {0,0}, FILE_GRAPHICS_WEST_EUROPE_CASTLE },
    { {0,0}, FILE_GRAPHICS_WEST_EUROPE_CASTLE_SHADOW },
};
static Part militia[] = {
    { {0,0}, FILE_GRAPHICS_MILITIA_IDLE }
};
static Part man_at_arms[] = {
    { {0,0}, FILE_GRAPHICS_MAN_AT_ARMS_IDLE }
};
static Part long_swordsman[] = {
    { {0,0}, FILE_GRAPHICS_LONG_SWORDSMAN_IDLE }
};
static Part forest_tree[] = {
    { {0,0}, FILE_GRAPHICS_FOREST_TREE },
    { {0,0}, FILE_GRAPHICS_FOREST_TREE_SHADOW },
};

static Parts GetFlag(void)
{
    const Parts parts = {
        flag, UTIL_LEN(flag)
    };
    return parts;
}

static Parts GetMaleVillager(void)
{
    const Parts parts = {
        villager_male, UTIL_LEN(villager_male)
    };
    return parts;
}

static Parts GetFemaleVillager(void)
{
    const Parts parts = {
        villager_female, UTIL_LEN(villager_female)
    };
    return parts;
}

Parts Parts_GetRedArrows(void)
{
    const Parts parts = {
        red_arrows, UTIL_LEN(red_arrows)
    };
    return parts;
}

Parts Parts_GetSmoke(void)
{
    const Parts parts = {
        smoke_a, UTIL_LEN(smoke_a)
    };
    return parts;
}

Parts Parts_GetFire(void)
{
    Parts parts = { NULL, 1 }; // NOTICE THE 1 PART - RANDOM FIRE IS CHOSEN.
    switch(Util_Rand() % 5)
    {
        case 0: parts.part = fire_a; break;
        case 1: parts.part = fire_b; break;
        case 2: parts.part = fire_c; break;
        case 3: parts.part = fire_d; break;
        case 4: parts.part = fire_e; break;
    }
    return parts;
}

Parts Parts_GetForestTree(void)
{
    const Parts parts = {
        forest_tree, UTIL_LEN(forest_tree)
    };
    return parts;
}

static Parts GetTownCenter(const Age age)
{
    Parts parts = { NULL, 0 };
    switch(age)
    {
    case AGE_1: parts.part = town_center_age1; parts.count = UTIL_LEN(town_center_age1); break;
    case AGE_2: parts.part = town_center_age2; parts.count = UTIL_LEN(town_center_age2); break;
    case AGE_3: parts.part = town_center_age3; parts.count = UTIL_LEN(town_center_age3); break;
    }
    return parts;
}

static Parts GetBarracks(const Age age)
{
    Parts parts = { NULL, 0 };
    switch(age)
    {
    case AGE_1: parts.part = barracks_age1; parts.count = UTIL_LEN(barracks_age1); break;
    case AGE_2: parts.part = barracks_age2; parts.count = UTIL_LEN(barracks_age2); break;
    case AGE_3: parts.part = barracks_age3; parts.count = UTIL_LEN(barracks_age3); break;
    }
    return parts;
}

static Parts GetMill(const Age age)
{
    Parts parts = { NULL, 0 };
    switch(age)
    {
        case AGE_1: parts.part = mill_age1; parts.count = UTIL_LEN(mill_age1); break;
        case AGE_2: parts.part = mill_age2; parts.count = UTIL_LEN(mill_age2); break;
        case AGE_3: parts.part = mill_age3; parts.count = UTIL_LEN(mill_age3); break;
    }
    return parts;
}

static Parts GetHouse(const Age age)
{
    Parts parts = { NULL, 0 };
    switch(age)
    {
        case AGE_1: parts.part = house_age1; parts.count = UTIL_LEN(house_age1); break;
        case AGE_2: parts.part = house_age2; parts.count = UTIL_LEN(house_age2); break;
        case AGE_3: parts.part = house_age3; parts.count = UTIL_LEN(house_age3); break;
    }
    return parts;
}

static Parts GetOutpost(void)
{
    const Parts parts = {
        outpost, UTIL_LEN(outpost)
    };
    return parts;
}

static Parts GetStoneCamp(void)
{
    const Parts parts = {
        stone_camp, UTIL_LEN(stone_camp)
    };
    return parts;
}

static Parts GetLumberCamp(void)
{
    const Parts parts = {
        lumber_camp, UTIL_LEN(lumber_camp)
    };
    return parts;
}

static Parts GetCastle(void)
{
    const Parts parts = {
        castle, UTIL_LEN(castle)
    };
    return parts;
}

static Parts GetMilitia(void)
{
    const Parts parts = {
        militia, UTIL_LEN(militia)
    };
    return parts;
}

static Parts GetManAtArms(void)
{
    const Parts parts = {
        man_at_arms, UTIL_LEN(man_at_arms)
    };
    return parts;
}

static Parts GetLongSwordsMan(void)
{
    const Parts parts = {
        long_swordsman, UTIL_LEN(long_swordsman)
    };
    return parts;
}

static Parts Copy(const Parts parts)
{
    Part* part = UTIL_ALLOC(Part, parts.count);
    for(int32_t i = 0; i < parts.count; i++)
        part[i] = parts.part[i];
    const Parts copy = { part, parts.count };
    return copy;
}

static Parts Lookup(const Button button, const Age age)
{
    switch(button.icon_type)
    {
    case ICONTYPE_COMMAND: // NO PARTS FOR BASIC UNIT COMMANDS
        break;
    case ICONTYPE_BUILD:
        switch(button.icon_build)
        {
        case ICONBUILD_BARRACKS    : return GetBarracks  (age);
        case ICONBUILD_MILL        : return GetMill      (age);
        case ICONBUILD_HOUSE       : return GetHouse     (age);
        case ICONBUILD_OUTPOST     : return GetOutpost   (   );
        case ICONBUILD_STONE_CAMP  : return GetStoneCamp (   );
        case ICONBUILD_LUMBER_CAMP : return GetLumberCamp(   );
        case ICONBUILD_TOWN_CENTER : return GetTownCenter(age);
        case ICONBUILD_CASTLE      : return GetCastle    (   );
        }
        break;
    case ICONTYPE_TECH:
        switch(button.icon_tech)
        {
        case ICONTECH_AGE_2                   :
        case ICONTECH_AGE_3                   :
        case ICONTECH_RESEARCH_MAN_AT_ARMS    :
        case ICONTECH_RESEARCH_LONG_SWORDSMAN : return GetFlag();
        }
        break;
    case ICONTYPE_UNIT:
        switch(button.icon_unit)
        {
        case ICONUNIT_MILITIA         : return GetMilitia       ();
        case ICONUNIT_MAN_AT_ARMS     : return GetManAtArms     ();
        case ICONUNIT_LONG_SWORDSMAN  : return GetLongSwordsMan ();
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

Parts Parts_FromButton(const Button button, const Age age)
{
    return Copy(Lookup(button, age));
}

void Parts_Free(const Parts parts)
{
    free(parts.part);
}
