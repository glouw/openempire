#include "Icon.h"

#include <SDL2/SDL.h>

#include "Util.h"

static const char hotkeys[] = {
    'Q', 'W', 'E', 'R', 'T',
    'A', 'S', 'D', 'F', 'G',
    'Z', 'X', 'C', 'V', 'B',
};

const char* Icon_GetHotkeys(void)
{
    return hotkeys;
}

int32_t Icon_GetHotkeysLen(void)
{
    return UTIL_LEN(hotkeys);
}

#define AGE_1               \
    ICON_BUILD_HOUSE,       \
    ICON_BUILD_MILL,        \
    ICON_BUILD_STONE_CAMP,  \
    ICON_BUILD_LUMBER_CAMP, \
    ICON_BUILD_BARRACKS,    \
    ICON_BUILD_OUTPOST,     \
    ICON_BUILD_TOWN_CENTER

#define AGE_2 \
    ICON_NONE

#define AGE_3 \
    ICON_NONE

#define AGE_4 \
    ICON_NONE

static const Icon age1[] = {
    AGE_1,
};

static const Icon age2[] = {
    AGE_1,
    AGE_2,
};

static const Icon age3[] = {
    AGE_1,
    AGE_2,
    AGE_3,
};

static const Icon age4[] = {
    AGE_1,
    AGE_2,
    AGE_3,
    AGE_4,
};

const Icon* Icon_GetBuilding(const int32_t age)
{
    const Icon* ages[] = {
        age1,
        age2,
        age3,
        age4,
    };
    return ages[age];
}

int32_t Icon_GetBuildingLen(const int32_t age)
{
    const int32_t lens[] = {
        UTIL_LEN(age1),
        UTIL_LEN(age2),
        UTIL_LEN(age3),
        UTIL_LEN(age4),
    };
    return lens[age];
}

Icon Icon_FromInput(const Input input, const Motive motive)
{
    switch(motive.action)
    {
    default:
    case ACTION_BUILD:
        if(input.key[SDL_SCANCODE_Q]) return ICON_BUILD_HOUSE;
        if(input.key[SDL_SCANCODE_W]) return ICON_BUILD_MILL;
        if(input.key[SDL_SCANCODE_E]) return ICON_BUILD_STONE_CAMP;
        if(input.key[SDL_SCANCODE_R]) return ICON_BUILD_LUMBER_CAMP;
        if(input.key[SDL_SCANCODE_T]) return ICON_BUILD_BARRACKS;
        if(input.key[SDL_SCANCODE_A]) return ICON_BUILD_OUTPOST;
        if(input.key[SDL_SCANCODE_S]) return ICON_BUILD_TOWN_CENTER;
        break;
    case ACTION_COMMAND:
        break;
    case ACTION_UNIT_TECH:
        break;
    }
    return ICON_NONE;
}
