#include "Channels.h"

#include "Util.h"

static void Reclip(const Channels channels, const Overview overview)
{
    for(int32_t j = 0; j < channels.count; j++)
    {
        const Tiles tiles = channels.tiles[j];
        const Rect bound = overview.channel_rects.rect[j];
        for(int32_t i = 0; i < tiles.count; i++)
            tiles.tile[i] = Tile_Clip(tiles.tile[i], bound);
    }
}

Channels Channels_Make(const Tiles tiles, const Overview overview)
{
    static Channels zero;
    Channels channels = zero;
    channels.count = overview.channel_rects.count;
    channels.tiles = UTIL_ALLOC(Tiles, channels.count);
    for(int32_t i = 0; i < channels.count; i++)
        channels.tiles[i] = Tiles_Copy(tiles);
    // Only reclip when screen is being subdivided into smaller windows (eg. channels).
    if(channels.count > 1)
        Reclip(channels, overview);
    return channels;
}

void Channels_Free(const Channels channels)
{
    for(int32_t i = 0; i < channels.count; i++)
        Tiles_Free(channels.tiles[i]);
}
