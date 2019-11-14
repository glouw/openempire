// Demo renderer for demo purposes.

#include "Video.h"

#include "Config.h"
#include "Interfac.h"
#include "Vram.h"

#include "Util.h"

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
        SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
        Text_Printf(video.text_small, video.renderer, video.bot_rite, POSITION_BOT_RITE, 0xFF, 0, "%d / %d", index, count);
        SDL_RenderPresent(video.renderer);
        SDL_Delay(20);
    }
}

static void RenderAnimationDemo(const Video video, const Animation animation, const Point point)
{
    static Rect zero;
    const bool flips[] = { false, true };
    for(int32_t j = 0; j < UTIL_LEN(flips); j++)
        for(int32_t i = 0; i < animation.count; i++)
        {
            const Tile tile = { NULL, animation.surface[i], animation.frame[i], point, {0,0}, 255, true, flips[j], false, zero };
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
    static Rect zero;
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
        for(int32_t j = 0; j < (int32_t) blendomatic.nr_tiles; j++)
        {
            const Mode mode = blendomatic.mode[i];
            const Tile tile = { NULL, mode.mask_demo[j], mode.frame, video.middle, {0,0}, 255, true, false, false, zero };
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
    static Rect zero;
    const Vram vram = Vram_Lock(video.canvas, video.xres, video.yres, video.cpu_count);
    Vram_Clear(vram, 0x0);
    for(int32_t index = 0; index < animation.count; index++)
    {
        const Point point = Point_Wrap(index, width, xres);
        const Tile tile = { NULL, animation.surface[index], animation.frame[index], point, {0,0}, 255, true, false, false, zero };
        Vram_DrawTile(vram, tile);
    }
    SDL_RenderCopy(video.renderer, video.canvas, NULL, NULL);
    Vram_Unlock(video.canvas);
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
