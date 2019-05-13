#pragma once

#include "Position.h"
#include "Point.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    TTF_Font* font;
    SDL_Color color;
    SDL_Rect size;
}
Text;

Text Text_Build(const char* const path, const int32_t size, const uint32_t color);

int32_t Text_Puts(const Text, SDL_Renderer* const, const Point, const Position, const int32_t alpha, const int32_t line, const char* const string);

int32_t Text_Printf(const Text, SDL_Renderer* const, const Point, const Position, const int32_t alpha, const int32_t line, const char* const fmt, ...);

void Text_Free(const Text);
