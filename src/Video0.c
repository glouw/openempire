#include "Video.h"

#include "Surface.h"
#include "Window.h"
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

void Video_PrintLobby(const Video video, const int32_t users_connected, const int32_t users, const Color color)
{
    const char* const string = Color_ToString(color);
    QuickClear(video);
    Text_Printf(video.text, video.renderer, video.middle, POSITION_MIDDLE, 0xFF, 0, video.title);
    Text_Printf(video.text, video.renderer, video.middle, POSITION_MIDDLE, 0xFF, 1, "%d / %d", users_connected, users);
    Text_Printf(video.text, video.renderer, video.middle, POSITION_MIDDLE, 0xFF, 2, "%s", string);
    Present(video);
}

Video Video_Setup(const int32_t xres, const int32_t yres, const char* const title)
{
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
    // Breaks sanitizer - but that's okay - renderer race conditions will not affect syncing P2P.
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

static void PrintPerformanceMonitor(const Video video, const Units units, const int32_t dt, const int32_t cycles)
{
    static int32_t dt_hold;
    if(cycles % 10 == 0)
        dt_hold = dt;
    const Point top_rite = { video.xres, 0 };
    Text_Printf(video.text_small, video.renderer, top_rite, POSITION_TOP_RITE, 0xFF, 0,
            "units.count   : %6d\n"
            "dt (ms) video : %6d\n"
            "cycles        : %6d\n", units.count, dt_hold, cycles);
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

void Video_Render(const Video video, const Units units, const int32_t dt, const int32_t cycles)
{
    CopyCanvas(video);
    PrintPerformanceMonitor(video, units, dt, cycles);
    PrintResources(video, units);
    PrintHotkeys(video);
    Present(video);
}
