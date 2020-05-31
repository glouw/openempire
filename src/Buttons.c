// * THESE BUTTON COLLECTIONS MUST BE BASE TEMPLATES WITHOUT UPGRADES.

#include "Buttons.h"

#include "Util.h"

#define BUILD_AGE_1                                              \
    { ICONTYPE_BUILD, { ICONBUILD_HOUSE       }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_OUTPOST     }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_MILL        }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_STONE_CAMP  }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_LUMBER_CAMP }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_BARRACKS    }, TRIGGER_NONE }

#define BUILD_AGE_2                                              \
    { ICONTYPE_BUILD, { ICONBUILD_STABLE      }, TRIGGER_NONE }  \

#define BUILD_AGE_3                                              \
    { ICONTYPE_BUILD, { ICONBUILD_TOWN_CENTER }, TRIGGER_NONE }, \
    { ICONTYPE_BUILD, { ICONBUILD_CASTLE      }, TRIGGER_NONE }

static const Button build_age1[] = { BUILD_AGE_1 };
static const Button build_age2[] = { BUILD_AGE_1, BUILD_AGE_2 };
static const Button build_age3[] = { BUILD_AGE_1, BUILD_AGE_2, BUILD_AGE_3 };

static const Button* GetBuilding(const Age age)
{
    const Button* ages[] = {
        build_age1,
        build_age2,
        build_age3,
    };
    return ages[age];
}

static int32_t GetBuildingLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(build_age1),
        UTIL_LEN(build_age2),
        UTIL_LEN(build_age3),
    };
    return lens[age];
}

#define BARRACKS_AGE_1 \
    { ICONTYPE_UNIT, { ICONUNIT_MILITIA  }, TRIGGER_NONE }, \
    { ICONTYPE_UNIT, { ICONUNIT_SPEARMAN }, TRIGGER_NONE }

#define BARRACKS_AGE_2 \
    { ICONTYPE_TECH, { ICONTECH_RESEARCH_MAN_AT_ARMS }, TRIGGER_UPGRADE_MILITIA }

#define BARRACKS_AGE_3 \
    { ICONTYPE_TECH, { ICONTECH_RESEARCH_PIKEMAN }, TRIGGER_UPGRADE_SPEARMAN }

static const Button barracks_age1[] = { BARRACKS_AGE_1 };
static const Button barracks_age2[] = { BARRACKS_AGE_1, BARRACKS_AGE_2 };
static const Button barracks_age3[] = { BARRACKS_AGE_1, BARRACKS_AGE_2, BARRACKS_AGE_3 };

static const Button* GetBarracks(const Age age)
{
    const Button* ages[] = {
        barracks_age1,
        barracks_age2,
        barracks_age3,
    };
    return ages[age];
}

static int32_t GetBarracksLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(barracks_age1),
        UTIL_LEN(barracks_age2),
        UTIL_LEN(barracks_age3),
    };
    return lens[age];
}

#define STABLE_AGE_2 \
    { ICONTYPE_UNIT, { ICONUNIT_SCOUT }, TRIGGER_NONE }

static const Button stable_age1[] = { STABLE_AGE_2 };
static const Button stable_age2[] = { STABLE_AGE_2 };
static const Button stable_age3[] = { STABLE_AGE_2 };

static const Button* GetStable(const Age age)
{
    const Button* ages[] = {
        stable_age1,
        stable_age2,
        stable_age3,
    };
    return ages[age];
}

static int32_t GetStableLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(stable_age1),
        UTIL_LEN(stable_age2),
        UTIL_LEN(stable_age3),
    };
    return lens[age];
}

#define TOWN_CENTER_AGE_1                                              \
    { ICONTYPE_UNIT, { ICONUNIT_MALE_VILLAGER   }, TRIGGER_NONE     }, \
    { ICONTYPE_UNIT, { ICONUNIT_FEMALE_VILLAGER }, TRIGGER_NONE     }, \
    { ICONTYPE_TECH, { ICONTECH_AGE_2           }, TRIGGER_AGE_UP_2 }

static const Button towncenter_age1[] = { TOWN_CENTER_AGE_1 };
static const Button towncenter_age2[] = { TOWN_CENTER_AGE_1 };
static const Button towncenter_age3[] = { TOWN_CENTER_AGE_1 };

static const Button* GetTownCenter(const Age age)
{
    const Button* ages[] = {
        towncenter_age1,
        towncenter_age2,
        towncenter_age3,
    };
    return ages[age];
}

static int32_t GetTownCenterLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(towncenter_age1),
        UTIL_LEN(towncenter_age2),
        UTIL_LEN(towncenter_age3),
    };
    return lens[age];
}

#define COMMAND_AGE_1 \
    { ICONTYPE_COMMAND, { ICONCOMMAND_AGGRO_MOVE }, TRIGGER_NONE }

static const Button command_age1[] = { COMMAND_AGE_1 };
static const Button command_age2[] = { COMMAND_AGE_1 };
static const Button command_age3[] = { COMMAND_AGE_1 };

static const Button* GetCommand(const Age age)
{
    const Button* ages[] = {
        command_age1,
        command_age2,
        command_age3,
    };
    return ages[age];
}

static int32_t GetCommandLen(const Age age)
{
    const int32_t lens[] = {
        UTIL_LEN(command_age1),
        UTIL_LEN(command_age2),
        UTIL_LEN(command_age3),
    };
    return lens[age];
}

Buttons Buttons_FromMotive(const Motive motive, const Age age)
{
    static Buttons zero;
    Buttons buttons = zero;
    switch(motive.action)
    {
    case ACTION_BUILD:
        buttons.button = GetBuilding(age);
        buttons.count = GetBuildingLen(age);
        break;
    case ACTION_COMMAND:
        switch(motive.type)
        {
        case TYPE_MILITIA:
        case TYPE_MAN_AT_ARMS:
        case TYPE_LONG_SWORDSMAN:
        case TYPE_SCOUT:
        case TYPE_SPEARMAN:
        case TYPE_PIKEMAN:
            buttons.button = GetCommand(age);
            buttons.count = GetCommandLen(age);
            break;
        // EXEMPT - PREFERABLE TO BE EXPLICIT. DEFAULT
        // WILL SILENTLY HIDE NEWLY ADDED TYPES.
        case TYPE_NONE:
        case TYPE_VILLAGER_MALE:
        case TYPE_VILLAGER_FEMALE:
        case TYPE_FIRE:
        case TYPE_SHADOW:
        case TYPE_SMOKE:
        case TYPE_OUTPOST:
        case TYPE_TOWN_CENTER:
        case TYPE_BERRY_BUSH:
        case TYPE_KNIGHT:
        case TYPE_GOLD_MINE:
        case TYPE_STONE_MINE:
        case TYPE_MINING_CAMP:
        case TYPE_MILL:
        case TYPE_LUMBER_CAMP:
        case TYPE_CASTLE:
        case TYPE_BARRACKS:
        case TYPE_STABLE:
        case TYPE_HOUSE:
        case TYPE_RUBBLE:
        case TYPE_TREE:
        case TYPE_FLAG:
        case TYPE_FOOD:
        case TYPE_WOOD:
        case TYPE_GOLD:
        case TYPE_STONE:
        case TYPE_COUNT:
            break;
        }
        break;
    case ACTION_UNIT_TECH:
        switch(motive.type)
        {
        case TYPE_BARRACKS:
            buttons.button = GetBarracks(age);
            buttons.count = GetBarracksLen(age);
            break;
        case TYPE_STABLE:
            buttons.button = GetStable(age);
            buttons.count = GetStableLen(age);
            break;
        case TYPE_TOWN_CENTER:
            buttons.button = GetTownCenter(age);
            buttons.count = GetTownCenterLen(age);
            break;
        case TYPE_NONE:
        case TYPE_MILITIA:
        case TYPE_MAN_AT_ARMS:
        case TYPE_SPEARMAN:
        case TYPE_VILLAGER_MALE:
        case TYPE_VILLAGER_FEMALE:
        case TYPE_FIRE:
        case TYPE_SHADOW:
        case TYPE_SMOKE:
        case TYPE_OUTPOST:
        case TYPE_BERRY_BUSH:
        case TYPE_KNIGHT:
        case TYPE_GOLD_MINE:
        case TYPE_STONE_MINE:
        case TYPE_MINING_CAMP:
        case TYPE_MILL:
        case TYPE_LUMBER_CAMP:
        case TYPE_CASTLE:
        case TYPE_HOUSE:
        case TYPE_RUBBLE:
        case TYPE_SCOUT:
        case TYPE_LONG_SWORDSMAN:
        case TYPE_PIKEMAN:
        case TYPE_TREE:
        case TYPE_FLAG:
        case TYPE_FOOD:
        case TYPE_WOOD:
        case TYPE_GOLD:
        case TYPE_STONE:
        case TYPE_COUNT:
            break;
        }
        break;
    case ACTION_NONE:
    case ACTION_COUNT:
        break;
    }
    return buttons;
}

bool Buttons_IsIndexValid(const Buttons buttons, const int32_t index)
{
    return index != -1
        && buttons.button != NULL
        && index < buttons.count;
}
