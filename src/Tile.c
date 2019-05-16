#include "Tile.h"

#include "Util.h"

static bool OnScreen(const Tile tile, const int32_t xres, const int32_t yres)
{
    return tile.iso_point.x - tile.frame.hotspot_x >= 0
        && tile.iso_point.y - tile.frame.hotspot_y >= 0
        && tile.iso_point.x - tile.frame.hotspot_x + tile.frame.width < xres
        && tile.iso_point.y - tile.frame.hotspot_y + tile.frame.height < yres;
}

static Tile Clip(Tile tile, const Overview overview)
{
    if(!OnScreen(tile, overview.xres, overview.yres))
        tile.needs_clipping = true;

    // XXX: Need a totally out of bounds one to ease render time.

    return tile;
}

static Tile Construct(const Overview overview, const Point cart_point, const Point cart_fractional, const Animation animation, const int32_t index)
{
    static Tile zero;
    Tile tile = zero;
    tile.frame = animation.frame[index];
    tile.surface = animation.surface[index];
    tile.iso_point = Overview_CartToIso(overview, cart_point);
    tile.iso_fractional = Point_ToIso(cart_fractional);
    return Clip(tile, overview);
}

Tile Tile_GetTerrain(const Overview overview, const Point cart_point, const Animation animation, const Terrain file)
{
    const int32_t bound = Util_SquareRoot(animation.count);
    const int32_t index = (cart_point.x % bound) + ((cart_point.y % bound) * bound);
    const Point cart_fractional = { 0,0 };
    Tile tile = Construct(overview, cart_point, cart_fractional, animation, index);
    tile.height = Terrain_GetHeight(file);
    return tile;
}

Tile Tile_GetGraphics(const Overview overview, const Point cart_point, const Point cart_fractional, const Animation animation, const Graphics file)
{
    const int32_t index = 1; // XXX... which one to use?
    Tile tile = Construct(overview, cart_point, cart_fractional, animation, index);
    tile.height = Graphics_GetHeight(file);
    return tile;
}

Point Tile_GetScreenCoords(const Tile tile, const int32_t x, const int32_t y)
{
    const Point coords = {
        x - tile.frame.hotspot_x + tile.iso_point.x + tile.iso_fractional.x,
        y - tile.frame.hotspot_y + tile.iso_point.y + tile.iso_fractional.y,
    };
    return coords;
}
