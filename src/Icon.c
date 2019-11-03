#include "Icon.h"

#include "Icons.h"
#include "Util.h"

#include <SDL2/SDL.h>

/* Hotkeys */

#define X_LIST \
    X( 0, SDL_SCANCODE_Q, 'Q') \
    X( 1, SDL_SCANCODE_W, 'W') \
    X( 2, SDL_SCANCODE_E, 'E') \
    X( 3, SDL_SCANCODE_R, 'R') \
    X( 4, SDL_SCANCODE_T, 'T') \
    X( 5, SDL_SCANCODE_A, 'A') \
    X( 6, SDL_SCANCODE_S, 'S') \
    X( 7, SDL_SCANCODE_D, 'D') \
    X( 8, SDL_SCANCODE_F, 'F') \
    X( 9, SDL_SCANCODE_G, 'G') \
    X(10, SDL_SCANCODE_Z, 'Z') \
    X(11, SDL_SCANCODE_X, 'X') \
    X(12, SDL_SCANCODE_C, 'C') \
    X(14, SDL_SCANCODE_V, 'V') \
    X(15, SDL_SCANCODE_B, 'B') \

static const char hotkeys[] = {
#define X(index, scancode, ch) ch,
    X_LIST
#undef X
};

int32_t Icon_ScancodeToIndex(const Input input)
{
#define X(index, scancode, ch) if(input.key[scancode]) return index;
    X_LIST
#undef X
    return -1;
}

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

#define BARRACKS_AGE_1 ICON_UNIT_MILITIA
#define BARRACKS_AGE_2 ICON_NONE
#define BARRACKS_AGE_3 ICON_NONE
#define BARRACKS_AGE_4 ICON_NONE

/* Barracks */

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

Icon Icon_FromInput(const Input input, const Motive motive)
{
    const int32_t age = 0; // XXX. SHOULD BE TOP LEVEL.
    const Icons icons = Icons_FromMotive(motive, age);
    const int32_t index = Icon_ScancodeToIndex(input);
    return (index != -1 && icons.icon != NULL && index < icons.count) ? icons.icon[index] : ICON_NONE;
}
