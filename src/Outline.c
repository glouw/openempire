#include "Outline.h"

bool Outline_IsTransparent(const Outline outline)
{
    const uint16_t transparent_flag = 0x8000;
    return outline.left_padding == transparent_flag || outline.right_padding == transparent_flag;
}

int32_t Outline_GetWidth(const Outline outline)
{
    return outline.left_padding + outline.right_padding;
}

int32_t Outline_GetUnpaddedSize(void)
{
    Outline dummy;
    return sizeof(dummy.left_padding) + sizeof(dummy.right_padding);
}
