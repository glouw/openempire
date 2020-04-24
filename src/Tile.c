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

// A---------+
// |         |
// |         |
// +---------B
Rect Tile_GetFrameOutline(const Tile tile)
{
    const Point dimensions = { tile.frame.width, tile.frame.height };
    const Point a = Tile_GetTopLeftCoords(tile);
    const Point b = Point_Add(a, dimensions);
    const Rect rect = { a, b };
    return rect;
}

// A---------+
// |         | WHERE (*) IS THE POINT OF INTEREST.
// |         | AND POINTS (A, B) ARE A BOUNDING RECTANGLE.
// |         |
// |     *   |
// |         |
// +---------B
bool Tile_ContainsPoint(const Tile tile, const Point point)
{
    const Rect rect = Tile_GetFrameOutline(tile);
    return Rect_ContainsPoint(rect, point);
}

Tile Tile_Clip(Tile tile, const Rect bound)
{
    const Rect outline = Tile_GetFrameOutline(tile);
    tile.needs_clipping = !Rect_OnScreen(outline, bound);
    tile.totally_offscreen = Rect_TotallyOffScreen(outline, bound);
    tile.bound = bound;
    return tile;
}

typedef struct
{
    int32_t index;
    bool flip_vert;
}
Dynamics;

static Tile Construct(const Overview overview, const Grid grid, const Point cart, const Point cart_grid_offset, const Animation animation, const Dynamics dynamics, const uint8_t height, Unit* const reference)
{
    static Tile zero;
    Tile tile = zero;
    tile.frame = animation.frame[dynamics.index];
    tile.surface = animation.surface[dynamics.index];
    tile.iso_pixel = Overview_CartToIso(overview, grid, cart);
    tile.iso_pixel_offset = Point_ToIso(cart_grid_offset);
    tile.flip_vert = dynamics.flip_vert;
    tile.height = height;
    tile.reference = reference;
    if(reference != NULL)
    {
        tile.is_floating = reference->is_floating;
        if(tile.is_floating)
            tile.height = FILE_PRIO_HIGHEST;
    }
    const Rect bound = {
        { 0, 0 },
        { overview.xres, overview.yres }
    };
    return Tile_Clip(tile, bound);
}

Tile Tile_GetTerrain(const Overview overview, const Grid grid, const Point cart, const Animation animation, const Terrain file)
{
    const int32_t bound = Util_Sqrt(animation.count);
    const int32_t index = (cart.x % bound) + ((cart.y % bound) * bound);
    const Point cart_grid_offset = { 0,0 };
    const uint8_t height = Terrain_GetHeight(file);
    const Dynamics dynamics = { index, false };
    return Construct(overview, grid, cart, cart_grid_offset, animation, dynamics, height, NULL);
}

static Dynamics GetDynamics(const Animation animation, Unit* const reference)
{
    Dynamics dynamics = { 0, false };
    if(reference->trait.is_single_frame)
    {
        if(reference->is_being_built)
        {
            const int32_t bound = 100;
            const int32_t percent = (bound * reference->health) / reference->trait.max_health;
            const int32_t index = (animation.count * percent) / bound;
            dynamics.index = index == animation.count ? index - 1 : index;
        }
        else
        {
            int32_t id = reference->parent
                ? reference->parent_id
                : reference->id;
            if(reference->is_floating)
                id = 0;
            dynamics.index = id % animation.count;
        }
    }
    else
    if(reference->trait.is_multi_state)
    {
        const int32_t frames_per_direction = Animation_GetFramesPerDirection(animation);
        const Direction fixed_dir = Direction_Fix(reference->dir, &dynamics.flip_vert);
        const bool is_decaying = reference->state == STATE_DECAY;
        const int32_t divisor = is_decaying
            ? CONFIG_ANIMATION_DECAY_DIVISOR
            : CONFIG_ANIMATION_DIVISOR;
        const int32_t ticks = reference->state_timer / divisor;
        const int32_t frame = ticks % frames_per_direction;
        dynamics.index = frames_per_direction * fixed_dir + frame;
    }
    else
    {
        const int32_t ticks = reference->state_timer / CONFIG_ANIMATION_DIVISOR;
        dynamics.index = ticks % animation.count;
    }
    return dynamics;
}

Tile Tile_GetGraphics(const Overview overview, const Grid grid, const Point cart, const Point cart_grid_offset, const Animation animation, Unit* const reference)
{
    const Dynamics dynamics = GetDynamics(animation, reference);
    const Point shifted = Unit_GetShift(reference, cart);
    const uint8_t height = Graphics_GetHeight(reference->file);
    Point offset = cart_grid_offset;
    // MINOR GRAPHICS TWEAK FOR RUBBLE - SEEMS LIKE SPRITE RUBBLE ARTWORK WAS ALWAYS BUGGED WITH A HALF GRID OFFSET.
    if(reference->trait.type == TYPE_RUBBLE)
    {
        const Point shift = { 0, grid.tile_cart_mid.y / 2 };
        offset = Point_Sub(offset, shift);
    }
    return Construct(overview, grid, shifted, offset, animation, dynamics, height, reference);
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

void Tile_Select(const Tile tile, const Color color)
{
    Unit* const unit = tile.reference;
    if(!Unit_IsExempt(unit) && !unit->is_floating)
        Unit_SetSelectedColor(unit, color);
#if 0
    Unit_Print(unit);
#endif
}
