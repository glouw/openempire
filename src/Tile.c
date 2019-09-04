#include "Tile.h"

#include "Rect.h"
#include "Util.h"
#include "Config.h"

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
    return Rect_ContainsPoint(rect, point);
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

static Tile Construct(const Overview overview, const Point cart, const Point cart_grid_offset, const Animation animation, const int32_t index, const bool flip_vert, const uint8_t height, Unit* const reference)
{
    static Tile zero;
    Tile tile = zero;
    tile.frame = animation.frame[index];
    tile.surface = animation.surface[index];
    tile.iso_pixel = Overview_CartToIso(overview, cart);
    tile.iso_pixel_offset = Point_ToIso(cart_grid_offset);
    tile.flip_vert = flip_vert;
    tile.height = height;
    tile.reference = reference;
    return Clip(tile, overview);
}

Tile Tile_GetTerrain(const Overview overview, const Point cart_point, const Animation animation, const Terrain file)
{
    const int32_t bound = Util_Sqrt(animation.count);
    const int32_t index = (cart_point.x % bound) + ((cart_point.y % bound) * bound);
    static Point zero;
    const Point cart_grid_offset = zero;
    const uint8_t height = Terrain_GetHeight(file);
    return Construct(overview, cart_point, cart_grid_offset, animation, index, false, height, NULL);
}

Tile Tile_GetGraphics(const Overview overview, const Point cart, const Point cart_grid_offset, const Animation animation, Unit* const reference)
{
    int32_t index = 0;
    bool flip_vert = false;
    if(reference->is_single_frame)
    {
        index = reference->id % animation.count;
        if(index == 0)
            index = 1;
    }
    else
    if(reference->is_rotatable)
    {
        const int32_t frames_per_direction = Animation_GetFramesPerDirection(animation);
        const Direction fixed_dir = Direction_Fix(reference->dir, &flip_vert);
        const int32_t divisor = reference->state == STATE_DECAY ? CONFIG_ANIMATION_DECAY_DIVISOR : CONFIG_ANIMATION_DIVISOR;
        const int32_t ticks = reference->state_timer / divisor;
        const int32_t frame = ticks % frames_per_direction;
        index = frames_per_direction * fixed_dir + frame;
    }
    else
    {
        const int32_t ticks = reference->state_timer / CONFIG_ANIMATION_DIVISOR;
        index = ticks % animation.count;
    }
    // A little unfortunate, but the hot spots for the terrain tiles are not centered.
    // Units must therefor be forced to the terrain tile positions.
    const Point south = { 0, 1 };
    const Point shifted = Point_Add(cart, south);
    const uint8_t height = Graphics_GetHeight(reference->file);
    return Construct(overview, shifted, cart_grid_offset, animation, index, flip_vert, height, reference);
}

Point Tile_GetHotSpotCoords(const Tile tile)
{
    return Point_Add(tile.iso_pixel, tile.iso_pixel_offset);
}

Point Tile_GetTopLeftCoords(const Tile tile)
{
    const Point hotspot = {
        tile.flip_vert ? (tile.frame.width - 1 - tile.frame.hotspot_x) : tile.frame.hotspot_x,
        tile.frame.hotspot_y
    };
    return Point_Sub(Tile_GetHotSpotCoords(tile), hotspot);
}

Point Tile_GetTopLeftOffsetCoords(const Tile tile, const int32_t x, const int32_t y)
{
    const Point point = {
        tile.flip_vert ? (tile.frame.width - 1 - x) : x, y
    };
    return Point_Add(point, Tile_GetTopLeftCoords(tile));
}

void Tile_Select(const Tile tile)
{
    if(!State_IsDead(tile.reference->state))
        tile.reference->is_selected = true;
}
