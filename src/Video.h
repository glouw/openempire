#pragma once

#include "Text.h"
#include "Overview.h"
#include "Color.h"
#include "Map.h"
#include "Units.h"
#include "Blendomatic.h"
#include "Data.h"
#include "Point.h"

#include <SDL2/SDL.h>

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* canvas;
    const char* title;
    Text text;
    Text text_small;
    int32_t xres;
    int32_t yres;
    SDL_Cursor* cursor;
    Point middle;
    Point bot_rite;
    Point top_left;
}
Video;

Video Video_Setup(const int32_t xres, const int32_t yres, const char* const title);

void Video_Free(const Video);

int32_t Video_Render(const Video, const Data, const Map, const Units, const Overview, const Input);

void Video_RenderDataDemo(const Video, const Data, const Color);

void Video_Present(const Video);
