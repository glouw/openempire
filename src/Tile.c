#include "Tile.h"

#include "Util.h"

static bool OnScreen(const Tile tile, const int32_t xres, const int32_t yres)
{
    return tile.point.x - tile.frame.hotspot_x >= 0
        && tile.point.y - tile.frame.hotspot_y >= 0
        && tile.point.x - tile.frame.hotspot_x + tile.frame.width < xres
        && tile.point.y - tile.frame.hotspot_y + tile.frame.height < yres;
}

static Tile Clip(Tile tile, const Overview overview)
{
    if(!OnScreen(tile, overview.xres, overview.yres))
        tile.needs_clipping = true;

    // XXX: Need a totally out of bounds one to ease render time.

    return tile;
}

static Tile Construct(const Overview overview, const Point point, const Animation animation, const int32_t index)
{
    static Tile zero;
    Tile tile = zero;
    tile.frame = animation.frame[index];
    tile.surface = animation.surface[index];
    tile.point = Overview_CartToIso(overview, point);
    return Clip(tile, overview);
}

Tile Tile_GetTerrain(const Overview overview, const Point point, const Animation animation, const Terrain file)
{
    const int32_t bound = Util_SquareRoot(animation.count);
    const int32_t index = (point.x % bound) + ((point.y % bound) * bound);
    Tile tile = Construct(overview, point, animation, index);
    tile.height = Terrain_GetHeight(file);
    return tile;
}

Tile Tile_GetGraphics(const Overview overview, const Point point, const Animation animation, const Graphics file)
{
    const int32_t index = 1; // XXX... which one to use?
    Tile tile = Construct(overview, point, animation, index);
    tile.height = Graphics_GetHeight(file);
    return tile;
}
