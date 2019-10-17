#include "Video.h"

#include "Surface.h"
#include "Point.h"
#include "Icon.h"
#include "Tile.h"
#include "Config.h"
#include "Interfac.h"
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
    const Point bot_left = { 0, yres };
    const Point top_rite = { xres, 0 };
    const Point top_left = { 0, 0 };
    SDL_Init(SDL_INIT_VIDEO);
    static Video zero;
    Video video = zero;
    video.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xres, yres, SDL_WINDOW_SHOWN);
    video.renderer = SDL_CreateRenderer(video.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    video.canvas = SDL_CreateTexture(video.renderer, SURFACE_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, xres, yres);
    video.title = title;
    video.text = Text_Build("art/blood-and-blade.ttf", 28, 0x00FF0000);
    video.text_small = Text_Build("art/TerminusTTF-4.47.0.ttf", 14, 0x00FFFF00);
    video.xres = xres;
    video.yres = yres;
    video.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    video.middle = middle;
    video.bot_rite = bot_rite;
    video.bot_left = bot_left;
    video.top_rite = top_rite;
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

// XXX. TO BE DEPRECATED.
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
        Text_Printf(video.text_small, video.renderer, video.bot_rite, POSITION_BOT_RITE, 0xFF, 0, "%d / %d", index, count);
        SDL_RenderPresent(video.renderer);
        SDL_Delay(20);
    }
}

// XXX. TO BE DEPRECATED.
static void RenderAnimationDemo(const Video video, const Animation animation, const Point point)
{
    const bool flips[] = { false, true };
    for(int32_t j = 0; j < UTIL_LEN(flips); j++)
    for(int32_t i = 0; i < animation.count; i++)
    {
        const Tile tile = { NULL, animation.surface[i], animation.frame[i], point, {0,0}, 255, true, flips[j], false };
        RenderDemoTile(video, tile, i, animation.count);
    }
}

// XXX. TO BE DEPRECATED.
static void RenderRegistrarDemo(const Video video, const Registrar registrar, const Color color, const Point point)
{
    for(int32_t i = 0; i < registrar.file_count; i++)
    {
        const int32_t file = registrar.files[i];
        if(file != FILE_NONE)
            RenderAnimationDemo(video, registrar.animation[color][file], point);
    }
}

// XXX. TO BE DEPRECATED.
static void RenderBlendomaticDemo(const Video video, const Blendomatic blendomatic)
{
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
    for(int32_t j = 0; j < (int32_t) blendomatic.nr_tiles; j++)
    {
        const Mode mode = blendomatic.mode[i];
        const Tile tile = { NULL, mode.mask_demo[j], mode.frame, video.middle, {0,0}, 255, true, false, false };
        RenderDemoTile(video, tile, j, blendomatic.nr_tiles);
    }
}

static void LayoutNumbers(const Video video, const Animation animation, const int32_t width, const int32_t xres)
{
    for(int32_t index = 0; index < animation.count; index++)
    {
        const Point point = Point_Wrap(index, width, xres);
        Text_Printf(video.text_small, video.renderer, point, POSITION_TOP_LEFT, 0xFF, 0, "%d", index);
    }
}

static void LayoutIcons(const Video video, const Animation animation, const int32_t width, const int32_t xres)
{
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres);
    Vram_Clear(vram, 0x0);
    for(int32_t index = 0; index < animation.count; index++)
    {
        const Point point = Point_Wrap(index, width, xres);
        const Tile tile = { NULL, animation.surface[index], animation.frame[index], point, {0,0}, 255, true, false, false };
        Vram_DrawTile(vram, tile);
    }
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
    Vram_Unlock(video.canvas);
}

// XXX. TO BE DEPRECATED.
static void RenderIcons(const Video video, const Registrar interfac, const Interfac file, const Color color)
{
    const int32_t width = CONFIG_ICON_SIZE;
    const int32_t xres = 10 * CONFIG_ICON_SIZE;
    const Animation animation = interfac.animation[color][file];
    LayoutIcons(video, animation, width, xres);
    LayoutNumbers(video, animation, width, xres);
    const char* const str = Interfac_GetString(file);
    Text_Printf(video.text_small, video.renderer, video.bot_left, POSITION_BOT_LEFT, 0xFF, 0, str);
    SDL_RenderPresent(video.renderer);
    for(Input input = Input_Ready(); !input.key[SDL_SCANCODE_RETURN]; input = Input_Pump(input))
    {
        if(input.done)
            return;
        SDL_Delay(1);
    }
    SDL_Delay(100);
}

// XXX. TO BE DEPRECATED.
void Video_RenderDataDemo(const Video video, const Data data, const Color color)
{
    const Interfac interfacs[] = {
        FILE_INTERFAC_BUILDING_ICONS,
        FILE_INTERFAC_COMMAND_ICONS,
        FILE_INTERFAC_TECH_ICONS,
        FILE_INTERFAC_UNIT_ICONS,
    };
    for(int32_t i = 0; i < UTIL_LEN(interfacs); i++)
        RenderIcons(video, data.interfac, interfacs[i], color);

    RenderRegistrarDemo(video, data.graphics, color, video.middle);
    RenderRegistrarDemo(video, data.terrain, color, video.middle);
    RenderBlendomaticDemo(video, data.blendomatic);
}

void Video_CopyCanvas(const Video video)
{
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
}

void Video_Present(const Video video)
{
    SDL_RenderPresent(video.renderer);
}

void Video_PrintHotkeys(const Video video)
{
    const char* hotkeys = Icon_GetHotkeys();
    for(int32_t index = 0; index < 15; index++)
    {
        const char hotkey = hotkeys[index];
        const Point offset = Point_Layout(index, video.xres, video.yres);
        Text_Printf(video.text_small, video.renderer, offset, POSITION_TOP_LEFT, 0xFF, 0, "%c", hotkey);
    }
}

void Video_Render(const Video video, const Data data, const Map map, const Units units, const Overview overview, const Input input, const Window window)
{
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres);
    const Tiles graphics_tiles = Tiles_PrepGraphics(data.graphics, overview, units, window.units);
    const Tiles terrain_tiles = Tiles_PrepTerrain(data.terrain, map, overview, window.terrain);
    const Lines blend_lines = Map_GetBlendLines(map, window.terrain);
    Lines_Sort(blend_lines);
    Vram_Clear(vram, 0x0);
    Vram_DrawUnits(vram, graphics_tiles);
    Vram_DrawUnitHealthBars(vram, graphics_tiles);
#if SANITIZE_THREAD == 0
    // Breaks sanitizer - but that's okay - renderer race conditions will not affect syncing P2P.
    Vram_DrawMap(vram, data.terrain, map, overview, data.blendomatic, input, blend_lines, terrain_tiles);
#endif
    Vram_DrawMouseTileSelect(vram, data.terrain, input, overview);
    Vram_DrawUnitSelections(vram, graphics_tiles);
    Vram_DrawSelectionBox(vram, overview, 0x00FFFFFF, input.l);
    Vram_DrawActionRow(vram, data.interfac, units.action, overview.color);
    Vram_DrawHud(vram, data.interfac);
    Vram_Unlock(video.canvas);
    Tiles_Free(graphics_tiles);
    Tiles_Free(terrain_tiles);
    Lines_Free(blend_lines);
}

void Video_PrintPerformanceMonitor(const Video video, const Units units, const int32_t dt, const int32_t cycles)
{
    static int32_t dt_hold;
    if(cycles % 10 == 0)
        dt_hold = dt;
    const Point top_rite = { video.xres, CONFIG_VIDEO_TEXT_START_TOP };
    Text_Printf(video.text_small, video.renderer, top_rite, POSITION_TOP_RITE, 0xFF, 0,
            "units.count   : %6d\n"
            "dt (ms) video : %6d\n"
            "cycles        : %6d\n", units.count, dt_hold, cycles);
}

void Video_PrintResources(const Video video, const Units units)
{
    const int32_t space = 77;
    const int32_t x0 = 27;
    const int32_t y0 = 11;
    const Point a = { x0 + 0 * space, y0 };
    const Point b = { x0 + 1 * space, y0 };
    const Point c = { x0 + 2 * space, y0 };
    const Point d = { x0 + 3 * space, y0 };
    const Point e = { x0 + 4 * space, y0 };
    Text_Printf(video.text_small, video.renderer, a, POSITION_TOP_LEFT, 0xFF, 0, "%6d", units.wood);
    Text_Printf(video.text_small, video.renderer, b, POSITION_TOP_LEFT, 0xFF, 0, "%6d", units.food);
    Text_Printf(video.text_small, video.renderer, c, POSITION_TOP_LEFT, 0xFF, 0, "%6d", units.gold);
    Text_Printf(video.text_small, video.renderer, d, POSITION_TOP_LEFT, 0xFF, 0, "%6d", units.stone);
    Text_Printf(video.text_small, video.renderer, e, POSITION_TOP_LEFT, 0xFF, 0, "%6d", units.population);
}
