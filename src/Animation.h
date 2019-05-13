#pragma once

#include "Frame.h"
#include "Drs.h"
#include "Slp.h"
#include "Image.h"
#include "Palette.h"
#include "Animation.h"

#include <SDL2/SDL.h>

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
