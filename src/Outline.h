#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t left_padding;
    uint16_t right_padding;
}
Outline;

bool Outline_IsTransparent(const Outline);

int32_t Outline_GetWidth(const Outline);

int32_t Outline_GetUnpaddedSize(void);
