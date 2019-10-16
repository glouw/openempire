#include "Icon.h"

#include "Util.h"

#define ICON_AGE_1 \
    ICON_BUILD_BARRACKS,\
    ICON_BUILD_MILL,\
    ICON_BUILD_PALISADE_WALL,\
    ICON_BUILD_HOUSE,\
    ICON_BUILD_OUTPOST,\
    ICON_BUILD_STONE_CAMP,\
    ICON_BUILD_LUMBER_CAMP,\

static const Icon Age1[] = {
    ICON_AGE_1
};

const Icon* Icon_GetAge1(void)
{
    return Age1;
}

int32_t Icon_GetAge1Len(void)
{
    return UTIL_LEN(Age1);
}
