#include "Channels.h"

#include "Util.h"

static void Reclip(const Channels channels, const Vram vram)
{
    for(int32_t j = 0; j < channels.count; j++)
    {
        const Tiles tiles = channels.tiles[j];
        const Rect bound = vram.channel_rects.rect[j];
        for(int32_t i = 0; i < tiles.count; i++)
            tiles.tile[i] = Tile_Clip(tiles.tile[i], bound);
    }
}

Channels Channels_Make(const Tiles tiles, const Vram vram)
{
    static Channels zero;
    Channels channels = zero;
    channels.count = vram.channel_rects.count;
    channels.tiles = UTIL_ALLOC(Tiles, channels.count);
    for(int32_t i = 0; i < channels.count; i++)
        channels.tiles[i] = Tiles_Copy(tiles);
    if(channels.count > 1)
        Reclip(channels, vram);
    return channels;
}

void Channels_Free(const Channels channels)
{
    for(int32_t i = 0; i < channels.count; i++)
        Tiles_Free(channels.tiles[i]);
    free(channels.tiles);
}
