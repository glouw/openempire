#include "Tile.h"

#include "Rect.h"
#include "Util.h"

bool Tile_IsHotspotInRect(const Tile tile, const Rect rect)
{
    const Point point = Tile_GetHotSpotCoords(tile);
    return point.x >= rect.a.x
        && point.y >= rect.a.y
        && point.x < rect.b.x
        && point.y < rect.b.y;
}

// a---------+
// |         |
// |         |
// +---------b

Rect Tile_GetFrameOutline(const Tile tile)
{
    const Point dimensions = { tile.frame.width, tile.frame.height };
    const Point a = Tile_GetTopLeftCoords(tile);
    const Point b = Point_Add(a, dimensions);
    const Rect rect = { a, b };
    return rect;
}

// a---------+
// |         | Where (*) is the point of interest.
// |         | and points (a, b) are a bounding rectangle.
// |         |
// |     *   |
// |         |
// +---------b

bool Tile_ContainsPoint(const Tile tile, const Point point)
{
    const Rect rect = Tile_GetFrameOutline(tile);
    return point.x >= rect.a.x
        && point.y >= rect.a.y
        && point.x < rect.b.x
        && point.y < rect.b.y;
}

// (0, 0)
// +-------------------+
// |                   | Where the inner box, bounded by points (a, b),
// |       a------+    | are within screen dimensions (0,0) and (xres, yres)
// |       |      |    |
// |       +------b    |
// |                   |
// +-------------------+ (xres, yres)

static bool OnScreen(const Tile tile, const int32_t xres, const int32_t yres)
{
    const Rect rect = Tile_GetFrameOutline(tile);
    return rect.a.x >= 0
        && rect.a.y >= 0
        && rect.b.x < xres
        && rect.b.y < yres;
}

static Tile Clip(Tile tile, const Overview overview)
{
    if(!OnScreen(tile, overview.xres, overview.yres))
        tile.needs_clipping = true;

    // XXX: Need a totally out of bounds one to ease render time.

    return tile;
}

Tile Tile_Construct(const Overview overview, const Point cart, const Point cart_grid_offset, const Animation animation, const int32_t index)
{
    static Tile zero;
    Tile tile = zero;
    tile.frame = animation.frame[index];
    tile.surface = animation.surface[index];
    tile.iso_pixel = Overview_CartToIso(overview, cart);
    tile.iso_pixel_offset = Point_ToIso(cart_grid_offset);
    return Clip(tile, overview);
}

Tile Tile_GetTerrain(const Overview overview, const Point cart_point, const Animation animation, const Terrain file)
{
    const int32_t bound = Util_Sqrt(animation.count);
    const int32_t index = (cart_point.x % bound) + ((cart_point.y % bound) * bound);
    const Point cart_grid_offset = { 0,0 };
    Tile tile = Tile_Construct(overview, cart_point, cart_grid_offset, animation, index);
    tile.height = Terrain_GetHeight(file);
    return tile;
}

Tile Tile_GetGraphics(const Overview overview, const Point cart, const Point cart_grid_offset, const Animation animation, const Graphics file)
{
    const int32_t index = 1; // XXX... which one to use?
    Tile tile = Tile_Construct(overview, cart, cart_grid_offset, animation, index);
    tile.height = Graphics_GetHeight(file);
    return tile;
}

Point Tile_GetHotSpotCoords(const Tile tile)
{
    const Point coords = {
        tile.iso_pixel.x + tile.iso_pixel_offset.x,
        tile.iso_pixel.y + tile.iso_pixel_offset.y,
    };
    return coords;
}

Point Tile_GetTopLeftCoords(const Tile tile)
{
    const Point hotspot = {
        tile.frame.hotspot_x,
        tile.frame.hotspot_y,
    };
    return Point_Sub(Tile_GetHotSpotCoords(tile), hotspot);
}

Point Tile_GetTopLeftOffsetCoords(const Tile tile, const int32_t x, const int32_t y)
{
    const Point point = { x, y };
    return Point_Add(point, Tile_GetTopLeftCoords(tile));
}
