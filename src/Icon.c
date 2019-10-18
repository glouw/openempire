#include "Icon.h"

#include "Util.h"

#define ICON_AGE_1 \
    ICON_BUILD_HOUSE,\
    ICON_BUILD_MILL,\
    ICON_BUILD_STONE_CAMP,\
    ICON_BUILD_LUMBER_CAMP,\
    ICON_BUILD_BARRACKS,\
    ICON_BUILD_PALISADE_WALL,\
    ICON_BUILD_OUTPOST,\

static const char hotkeys[] = {
    'Q', 'W', 'E', 'R', 'T',
    'A', 'S', 'D', 'F', 'G',
    'Z', 'X', 'C', 'V', 'B',
};

static const Icon age1[] = {
    ICON_AGE_1
};

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
