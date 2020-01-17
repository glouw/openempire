#pragma once

#include "Text.h"
#include "Overview.h"
#include "Grid.h"
#include "Color.h"
#include "Map.h"
#include "Units.h"
#include "Data.h"

#include <SDL2/SDL.h>

typedef struct
{
    const char* title;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* canvas;
    SDL_Cursor* cursor;
    Text text;
    Text text_small;
    int32_t xres;
    int32_t yres;
    Point middle;
    Point bot_left;
    Point bot_rite;
    Point top_left;
    Point top_rite;
    int32_t cpu_count;
}
Video;

Video Video_Setup(const int32_t xres, const int32_t yres, const char* const title);

void Video_Free(const Video);

void Video_Draw(const Video, const Data, const Map, const Units units, const Units floats, const Overview, const Grid);

void Video_RenderDataDemo(const Video, const Data, const Color);

void Video_PrintPerformanceMonitor(const Video, const Units, const int32_t dt, const int32_t cycles);

void Video_Render(const Video, const Units, const Map, const int32_t dt, const int32_t cycles);

void Video_PrintLobby(const Video, const int32_t users_connected, const int32_t users, const Color, const int32_t loops);
