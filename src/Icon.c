#include "Icon.h"

#include <SDL2/SDL.h>

#include "Util.h"

#define X_LIST \
    X(ICON_BUILD_HOUSE,       SDL_SCANCODE_Q, 'Q') \
    X(ICON_BUILD_MILL,        SDL_SCANCODE_W, 'W') \
    X(ICON_BUILD_STONE_CAMP,  SDL_SCANCODE_E, 'E') \
    X(ICON_BUILD_LUMBER_CAMP, SDL_SCANCODE_R, 'R') \
    X(ICON_BUILD_BARRACKS,    SDL_SCANCODE_T, 'T') \
    X(ICON_BUILD_OUTPOST,     SDL_SCANCODE_A, 'A') \
    X(ICON_BUILD_TOWN_CENTER, SDL_SCANCODE_S, 'S')

static const char hotkeys[] = {
#define X(icon, scancode, c) c,
    X_LIST
#undef X
};

static const Icon age1[] = {
#define X(icon, scancode, c) icon,
    X_LIST
#undef X
};

Icon Icon_FromInput(const Input input)
{
#define X(icon, scancode, c) if(input.key[scancode]) return icon;
    X_LIST
#undef X
    return ICON_NONE;
}

const char* Icon_GetHotkeys(void)
{
    return hotkeys;
}

int32_t Icon_GetHotkeysLen(void)
{
    return UTIL_LEN(hotkeys);
}

const Icon* Icon_GetAge1(void)
{
    return age1;
}

int32_t Icon_GetAge1Len(void)
{
    return UTIL_LEN(age1);
}
