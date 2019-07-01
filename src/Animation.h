#pragma once

#include "Frame.h"
#include "Drs.h"
#include "Slp.h"
#include "Image.h"
#include "Palette.h"
#include "Animation.h"

#include <SDL2/SDL.h>

#define ANIMATION_DIVISOR (5)
#define ANIMATION_DECAY_DIVISOR (500 * ANIMATION_DIVISOR)

typedef struct
{
    SDL_Surface** surface;
    Frame* frame;
    Image* image;
    int32_t count;
}
Animation;

Animation Animation_Get(const Slp, const Palette, const Color);

void Animation_Free(const Animation);

int32_t Animation_GetFramesPerDirection(const Animation);
