#include "Channels.h"

#include "Util.h"

static void Prep(const Channels channels, const Overview overview)
{
    for(int32_t j = 0; j < channels.count; j++)
    {
        Tiles tiles = channels.tiles[j];
        const Rect bound = overview.rects.rect[j];
        for(int32_t i = 0; i < tiles.count; i++)
        {
            Tile* tile = &tiles.tile[i];
            const Rect outline = Tile_GetFrameOutline(*tile);
            tile->needs_clipping = !Rect_OnScreen(outline, bound);
            tile->totally_offscreen = Rect_TotallyOffScreen(outline, bound);
            tile->bound = bound;
        }
    }
}

Channels Channels_Make(const Tiles tiles, const Overview overview)
{
    static Channels zero;
    Channels channels = zero;
    channels.count = overview.rects.count;
    channels.tiles = UTIL_ALLOC(Tiles, channels.count);
    for(int32_t i = 0; i < channels.count; i++)
        channels.tiles[i] = Tiles_Copy(tiles);
    Prep(channels, overview);
    return channels;
}

void Channels_Free(const Channels channels)
{
    for(int32_t i = 0; i < channels.count; i++)
        Tiles_Free(channels.tiles[i]);
}
