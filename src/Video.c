#include "Video.h"

#include "Surface.h"
#include "Point.h"
#include "Tile.h"
#include "Vram.h"

#include "Util.h"

static void PrintTitle(const Video video)
{
    Text_Printf(video.text, video.renderer, video.middle, POSITION_MIDDLE, 0xFF, 0, video.title);
    SDL_RenderPresent(video.renderer);
}

Video Video_Setup(const int32_t xres, const int32_t yres, const char* const title)
{
    const Point middle = { xres / 2, yres / 2 };
    const Point bot_rite = { xres, yres };
    const Point top_left = { 0, 0 };
    SDL_Init(SDL_INIT_VIDEO);
    static Video zero;
    Video video = zero;
    video.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xres, yres, SDL_WINDOW_SHOWN);
    video.renderer = SDL_CreateRenderer(video.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    video.canvas = SDL_CreateTexture(video.renderer, SURFACE_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, xres, yres);
    video.title = title;
    video.text = Text_Build("art/blood_and_blade.ttf", 50, 0x00FF0000);
    video.text_small = Text_Build("art/blood_and_blade.ttf", 15, 0x00FFFF00);
    video.xres = xres;
    video.yres = yres;
    video.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    video.middle = middle;
    video.bot_rite = bot_rite;
    video.top_left = top_left;
    PrintTitle(video);
    SDL_SetCursor(video.cursor);
    return video;
}

void Video_Free(const Video video)
{
    SDL_DestroyWindow(video.window);
    SDL_DestroyRenderer(video.renderer);
    SDL_DestroyTexture(video.canvas);
    SDL_FreeCursor(video.cursor);
    Text_Free(video.text);
    Text_Free(video.text_small);
    SDL_Quit();
}

static void RenderDemoTile(const Video video, const Tile tile, const int32_t index, const int32_t count)
{
    const Input input = Input_Ready();
    if(!input.done)
    {
        const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres);
        Vram_Clear(vram, 0x0);
        Vram_DrawTile(vram, tile);
        Vram_DrawCross(vram, video.middle, 5, 0x00FF0000);
        Vram_Unlock(video.canvas);
        SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
        Text_Printf(video.text, video.renderer, video.bot_rite, POSITION_BOT_RITE, 0xFF, 0, "%d / %d", index, count);
        SDL_RenderPresent(video.renderer);
        SDL_Delay(20);
    }
}

static void RenderAnimationDemo(const Video video, const Animation animation, const Point point)
{
    const bool flips[] = { false, true };
    for(int32_t j = 0; j < UTIL_LEN(flips); j++)
    for(int32_t i = 0; i < animation.count; i++)
    {
        const Tile tile = { animation.frame[i], point, {0,0}, animation.surface[i], true, 255, NULL, flips[j] };
        RenderDemoTile(video, tile, i, animation.count);
    }
}

static void RenderRegistrarDemo(const Video video, const Registrar registrar, const Color color, const Point point)
{
    for(int32_t i = 0; i < registrar.file_count; i++)
    {
        const int32_t file = registrar.files[i];
        if(file != FILE_NONE)
            RenderAnimationDemo(video, registrar.animation[color][file], point);
    }
}

static void RenderBlendomaticDemo(const Video video, const Blendomatic blendomatic)
{
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
    for(int32_t j = 0; j < (int32_t) blendomatic.nr_tiles; j++)
    {
        const Mode mode = blendomatic.mode[i];
        const Tile tile = { mode.frame, video.middle, {0,0}, mode.mask_demo[j], true, 255, NULL, false };
        RenderDemoTile(video, tile, j, blendomatic.nr_tiles);
    }
}

void Video_RenderDataDemo(const Video video, const Data data, const Color color)
{
    RenderRegistrarDemo(video, data.interfac, color, video.top_left);
    RenderRegistrarDemo(video, data.graphics, color, video.middle);
    RenderRegistrarDemo(video, data.terrain, color, video.middle);
    RenderBlendomaticDemo(video, data.blendomatic);
}

static void PrintPerformanceMonitor(const Video video, const Units units, const int32_t dt, const int32_t cycle)
{
    int32_t kb = (units.count * sizeof(Unit)) / 1000;
    int32_t l1 = 0;
    int32_t l2 = 0;
    int32_t l3 = 0;
    while(kb > 0 && l1 <   32) { l1++; kb--; }
    while(kb > 0 && l2 <  256) { l2++; kb--; }
    while(kb > 0 && l3 < 3072) { l3++; kb--; }
    Text_Printf(video.text_small, video.renderer, video.top_left, POSITION_TOP_LEFT, 0xFF, 0,
            "Assuming i5-3320M\n"
            "DT : %2d ms\n"
            "L1 : %4dK ::   32K\n"
            "L2 : %4dK ::  256K\n"
            "L3 : %4dK :: 3072K\n"
            "cycle: %4d", dt, l1, l2, l3, cycle);
}

static void CopyCanvas(const Video video)
{
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
}

static void Present(const Video video)
{
    SDL_RenderPresent(video.renderer);
}

void Video_Render(const Video video, const Data data, const Map map, const Units units, const Overview overview, const Input input, const int32_t cycles)
{
    const int32_t t0 = SDL_GetTicks();
    const Quad quad = Overview_GetRenderBox(overview, -200);
    const Points render_points = Quad_GetRenderPoints(quad);
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres);
    const Tiles graphics_tiles = Tiles_PrepGraphics(data.graphics, overview, units, render_points);
    const Tiles terrain_tiles = Tiles_PrepTerrain(data.terrain, map, overview, render_points);
    const Lines blend_lines = Map_GetBlendLines(map, render_points);
    Lines_Sort(blend_lines);
    Vram_Clear(vram, 0x0);
    Vram_DrawUnits(vram, graphics_tiles);
    Vram_DrawUnitHealthBars(vram, graphics_tiles);
    Vram_DrawMap(vram, data.terrain, map, overview, data.blendomatic, input, blend_lines, terrain_tiles);
    Vram_DrawMouseTileSelect(vram, data.terrain, input, overview);
    Vram_DrawUnitSelections(vram, graphics_tiles);
    Vram_DrawSelectionBox(vram, overview, 0x00FFFFFF, input.l);
    Vram_DrawCross(vram, video.middle, 5, 0x00FF0000);
    Vram_Unlock(video.canvas);
    Points_Free(render_points);
    Tiles_Free(graphics_tiles);
    Tiles_Free(terrain_tiles);
    Lines_Free(blend_lines);
    const int32_t t1 = SDL_GetTicks();
    const int32_t dt = t1 - t0;
    static int32_t dt_hold;
    if(cycles % 10 == 0)
        dt_hold = dt;
    CopyCanvas(video);
    PrintPerformanceMonitor(video, units, dt_hold, cycles);
    Present(video);
}
