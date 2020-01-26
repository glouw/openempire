#include "Video.h"

#include "Surface.h"
#include "Window.h"
#include "Interfac.h"
#include "Button.h"
#include "Config.h"
#include "Vram.h"

#include "Util.h"

static void Present(const Video video)
{
    SDL_RenderPresent(video.renderer);
}

static void QuickClear(const Video video)
{
    SDL_SetRenderDrawColor(video.renderer, 0, 0, 0, 0);
    SDL_RenderClear(video.renderer);
}

void Video_PrintLobby(const Video video, const int32_t users_connected, const int32_t users, const Color color, const int32_t loops)
{
    const char* const string = (color == COLOR_GAIA) ? "Loading" : Color_ToString(color);
    QuickClear(video);
    const char* const periods[] = { "", ".", "..", "...", "....", "....." };
    const int32_t period = 10;
    const int32_t index = (loops % (period * UTIL_LEN(periods))) / period;
    Text_Printf(video.text, video.renderer, video.middle, POSITION_MIDDLE, 0xFF, 0,
            "%s\n"
            "%d / %d\n"
            "%s\n"
            "%s\n",
            video.title,
            users_connected, users,
            string,
            periods[index]);
    Present(video);
}

Video Video_Setup(const int32_t xres, const int32_t yres, const char* const title)
{
    const char* const terminus = "TerminusTTF-4.47.0.ttf";
    const Point middle = { xres / 2, yres / 2 };
    const Point bot_rite = { xres, yres };
    const Point bot_left = { 0, yres };
    const Point top_rite = { xres, 0 };
    const Point top_left = { 0, 0 };
    static Video zero;
    Video video = zero;
    video.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xres, yres, SDL_WINDOW_SHOWN);
    video.renderer = SDL_CreateRenderer(video.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    video.canvas = SDL_CreateTexture(video.renderer, SURFACE_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, xres, yres);
    video.title = title;
    video.text = Text_Build(terminus, 28, 0x00FF0000);
    video.text_small = Text_Build(terminus, 14, 0x00FFFF00);
    video.xres = xres;
    video.yres = yres;
    video.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    video.middle = middle;
    video.bot_rite = bot_rite;
    video.bot_left = bot_left;
    video.top_rite = top_rite;
    video.top_left = top_left;
    video.cpu_count = SDL_GetCPUCount();
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

static void CopyCanvas(const Video video)
{
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
}

static void PrintHotkeys(const Video video)
{
    const char* hotkeys = Button_GetHotkeys();
    const int32_t len = Button_GetHotkeysLen();
    for(int32_t index = 0; index < len; index++)
    {
        const char hotkey = hotkeys[index];
        const Point offset = Point_Layout(index, video.xres, video.yres);
        Text_Printf(video.text_small, video.renderer, offset, POSITION_TOP_LEFT, 0xFF, 0, "%c", hotkey);
    }
}

void Video_Draw(const Video video, const Data data, const Map map, const Units units, const Units floats, const Overview overview, const Grid grid)
{
    const Window window = Window_Make(overview, grid);
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres, video.cpu_count);
    const Tiles graphics_tiles = Tiles_PrepGraphics(data.graphics, overview, grid, units, window.units);
    const Tiles graphics_tiles_floats = Tiles_PrepGraphics(data.graphics, overview, grid, floats, window.units);
    const Tiles terrain_tiles = Tiles_PrepTerrain(data.terrain, map, overview, grid, window.terrain);
    const Lines blend_lines = Map_GetBlendLines(map, window.terrain);
    Lines_Sort(blend_lines);
    Vram_Clear(vram, 0x0);
    Vram_DrawUnits(vram, graphics_tiles);
    Vram_DrawUnitHealthBars(vram, graphics_tiles);
#if SANITIZE_THREAD == 0
    // BREAKS SANITIZER - BUT THAT'S OKAY - RENDERER RACE CONDITIONS WILL NOT AFFECT SYNCING P2P.
    Vram_DrawMap(vram, data.terrain, map, overview, grid, data.blendomatic, blend_lines, terrain_tiles);
#endif
    Vram_DrawUnits(vram, graphics_tiles_floats);
    Vram_DrawMouseTileSelect(vram, data.terrain, overview, grid);
    Vram_DrawUnitSelections(vram, graphics_tiles);
    const bool should_draw = overview.event.mouse_l && !overview.event.key_left_shift;
    Vram_DrawSelectionBox(vram, overview, 0x00FFFFFF, should_draw);
    Vram_DrawMotiveRow(vram, data.interfac, units.share);
    Vram_DrawHud(vram, data.interfac);
    Vram_DrawCross(vram, video.middle, 5, 0x00FF0000);
    Vram_Unlock(video.canvas);
    Vram_Free(vram);
    Tiles_Free(graphics_tiles);
    Tiles_Free(graphics_tiles_floats);
    Tiles_Free(terrain_tiles);
    Lines_Free(blend_lines);
    Window_Free(window);
}

static void PrintPerformanceMonitor(const Video video, const Units units, const int32_t dt, const int32_t cycles, const int32_t ping)
{
    static int32_t dt_hold;
    if(cycles % 10 == 0)
        dt_hold = dt;
    const Point top_rite = { video.xres, 0 };
    Text_Printf(video.text_small, video.renderer, top_rite, POSITION_TOP_RITE, 0xFF, 0,
            "units.count   : %6d\n"
            "ping          : %6d\n"
            "dt (ms) video : %6d\n"
            "cycles        : %6d\n", units.count, ping, dt_hold, cycles);
}

static void PrintResources(const Video video, const Units units)
{
    const int32_t space = 77;
    const int32_t x0 = 27;
    const int32_t y0 = 11;
    typedef struct
    {
        Point point;
        int32_t amount;
    }
    Display;
    const Display displays[] = {
        { { x0 + 0 * space, y0 }, units.share.status.wood  },
        { { x0 + 1 * space, y0 }, units.share.status.food  },
        { { x0 + 2 * space, y0 }, units.share.status.gold  },
        { { x0 + 3 * space, y0 }, units.share.status.stone },
        { { x0 + 4 * space, y0 }, units.share.status.population },
    };
    for(int32_t i = 0; i < UTIL_LEN(displays); i++)
    {
        const Display display = displays[i];
        Text_Printf(video.text_small, video.renderer, display.point, POSITION_TOP_LEFT, 0xFF, 0, "%6d", display.amount);
    }
}

// * DIM MUST BE POWER OF TWO (64, 128, 256, ETC).
// * BLENDMODE BLEND OPERATES LIKE:
//     DSTRGB = (SRCRGB * SRCA) + (DSTRGB * (1-SRCA))
//     DSTA = SRCA + (DSTA * (1-SRCA))
//   SO SET SRCA TO 0XFF TO HAVE PIXEL APPEAR ON MINIMAP.
static void DrawMiniMap(const Video video, const Units units, const Map map)
{
    const int32_t xres = 2 * map.size;
    const int32_t yres = 1 * map.size;
    SDL_Texture* const texture = SDL_CreateTexture(video.renderer, SURFACE_PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, xres, yres);
    Vram vram = Vram_Lock(texture, xres, yres, 1);
    Vram_Clear(vram, 0x00000000);
    Vram_DrawMiniMap(vram, units, map);
    Vram_Unlock(texture);
    Vram_Free(vram);
    const SDL_Rect dest = {
        video.middle.x - xres / 2,
        video.middle.y - yres / 2,
        xres,
        yres,
    };
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(video.renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

void Video_Render(const Video video, const Units units, const Overview overview, const Map map, const int32_t dt, const int32_t cycles, const int32_t ping)
{
    CopyCanvas(video);
    PrintPerformanceMonitor(video, units, dt, cycles, ping);
    PrintResources(video, units);
    PrintHotkeys(video);
    if(overview.event.tab)
        DrawMiniMap(video, units, map);
    Present(video);
}

static void RenderDemoTile(const Video video, const Tile tile, const int32_t index, const int32_t count)
{
    const Input input = Input_Ready();
    if(!input.done)
    {
        const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres, video.cpu_count);
        Vram_Clear(vram, 0x0);
        Vram_DrawTile(vram, tile);
        Vram_DrawCross(vram, video.middle, 5, 0x00FF0000);
        Vram_Unlock(video.canvas);
        Vram_Free(vram);
        SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
        Text_Printf(video.text_small, video.renderer, video.bot_rite, POSITION_BOT_RITE, 0xFF, 0, "%d / %d", index, count);
        SDL_RenderPresent(video.renderer);
        SDL_Delay(20);
    }
}

static void RenderAnimationDemo(const Video video, const Animation animation, const Point point)
{
    const Rect bound = { { 0,0 }, { video.xres, video.yres } };
    const bool flips[] = { false, true };
    for(int32_t j = 0; j < UTIL_LEN(flips); j++)
    for(int32_t i = 0; i < animation.count; i++)
    {
        const Tile tile = { NULL, animation.surface[i], animation.frame[i], point, {0,0}, 255, true, flips[j], false, bound, false };
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
    const Rect bound = { { 0,0 }, { video.xres, video.yres } };
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
    for(int32_t j = 0; j < (int32_t) blendomatic.nr_tiles; j++)
    {
        const Mode mode = blendomatic.mode[i];
        const Tile tile = { NULL, mode.mask_demo[j], mode.frame, video.middle, {0,0}, 255, true, false, false, bound, false };
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
    const Rect bound = { { 0,0 }, { video.xres, video.yres } };
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres, video.cpu_count);
    Vram_Clear(vram, 0x0);
    for(int32_t index = 0; index < animation.count; index++)
    {
        const Point point = Point_Wrap(index, width, xres);
        const Tile tile = { NULL, animation.surface[index], animation.frame[index], point, {0,0}, 255, true, false, false, bound, false };
        Vram_DrawTile(vram, tile);
    }
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
    Vram_Unlock(video.canvas);
    Vram_Free(vram);
}

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
