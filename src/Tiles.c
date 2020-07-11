#include "Tiles.h"

#include "Util.h"
#include "Rect.h"
#include "Surface.h"
#include "Config.h"

static int32_t CompareByY(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    const Point pa = Point_ToIso(aa->reference->cell);
    const Point pb = Point_ToIso(bb->reference->cell);
    return pa.y < pb.y;
}

static int32_t CompareBySurface(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    SDL_Surface* sa = aa->surface;
    SDL_Surface* sb = bb->surface;
    return sa < sb;
}

static int32_t CompareByHeight(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    const uint8_t ha = aa->height;
    const uint8_t hb = bb->height;
    return ha < hb;
}

static int32_t CompareByDetail(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    const bool da = aa->reference->trait.is_detail;
    const bool db = bb->reference->trait.is_detail;
    return da < db;
}

static int32_t CompareByDetailReversed(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    const bool da = aa->reference->trait.is_detail;
    const bool db = bb->reference->trait.is_detail;
    return da > db;
}

static void Sort(const Tiles tiles)
{
    UTIL_SORT(tiles.tile, tiles.count, CompareBySurface);
    UTIL_SORT(tiles.tile, tiles.count, CompareByDetail);
    UTIL_SORT(tiles.tile, tiles.count, CompareByY);
}

void Tiles_SortByHeight(const Tiles tiles)
{
    UTIL_SORT(tiles.tile, tiles.count, CompareByHeight);
}

void Tiles_SortByDetailReversed(const Tiles tiles)
{
    UTIL_SORT(tiles.tile, tiles.count, CompareByDetailReversed);
}

Tiles Tiles_PrepGraphics(const Registrar graphics, const Overview overview, const Grid grid, const Units units, const Points points)
{
    Tile* const tile = UTIL_ALLOC(Tile, units.count);
    int32_t unit_count = 0;
    for(int32_t i = 0; i < points.count; i++)
    {
        const Point point = points.point[i];
        const Stack stack = Units_GetStackCart(units, point);
        for(int32_t j = 0; j < stack.count; j++)
        {
            Unit* const ref = stack.reference[j];
            if(!ref->is_already_tiled)
            {
                ref->is_already_tiled = true;
                if(!ref->is_floating)
                {
                    // SKIP THE RENDER FOR CHILDREN OF BUILDINGS BEING BUILT.
                    if(ref->trait.is_inanimate
                    && ref->parent
                    && ref->parent->is_being_built)
                        continue;
                    // SKIP THE RALLY POINT RENDER FOR WAY POINT FLAGS THAT ARE NOT SELECTED.
                    if(ref->parent
                    && ref->parent->rally
                    && Unit_IsSelectedByColor(ref->parent, overview.color) == false
                    && ref->file == FILE_GRAPHICS_WAYPOINT_FLAG)
                        continue;
                }
                const Graphics file = Unit_IsConstruction(ref)
                    ? Unit_GetConstructionFile(ref)
                    : ref->file;
                const Animation animation = graphics.animation[ref->color][file];
                const Point overrider = ref->trait.is_inanimate ? ref->cart : point;
                tile[unit_count++] = Tile_GetGraphics(overview, grid, overrider, ref->cart_grid_offset, animation, ref);
            }
        }
    }
    Units_ResetTiled(units);
    const Tiles tiles = { tile, unit_count };
    Sort(tiles);
    return tiles;
}

void Tiles_Free(const Tiles tiles)
{
    free(tiles.tile);
}

static int32_t CompareByTileSurface(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    return aa->surface < bb->surface;
}

static void SortByTileSurface(const Tiles tiles)
{
    UTIL_SORT(tiles.tile, tiles.count, CompareByTileSurface);
}

Tiles Tiles_PrepTerrain(const Registrar terrain, const Map map, const Overview overview, const Grid grid, const Points points)
{
    Tile* const tile = UTIL_ALLOC(Tile, points.count);
    for(int32_t i = 0; i < points.count; i++)
    {
        const Point point = points.point[i];
        const Terrain file = Map_GetTerrainFile(map, point);
        if(file != FILE_TERRAIN_NONE)
        {
            const Animation animation = terrain.animation[COLOR_GAIA][file];
            tile[i] = Tile_GetTerrain(overview, grid, point, animation, file);
        }
    }
    const Tiles tiles = { tile, points.count };
    SortByTileSurface(tiles);
    return tiles;
}

Tile Tiles_Get(const Tiles tiles, const Overview overview, const Grid grid)
{
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        const int32_t width = CONFIG_TILES_CURSOR_SELECT_WIDTH;
        for(int32_t y = -width; y < width; y++)
        for(int32_t x = -width; x < width; x++)
        {
            const Point shift = { x, y };
            const Point cursor = Point_Add(overview.mouse_cursor, shift);
            if(Tile_ContainsPoint(tile, cursor))
            {
                const Rect rect = Tile_GetFrameOutline(tile);
                const Point origin_click = Point_Sub(cursor, rect.a);
                const uint32_t pixel = Surface_GetPixel(tile.surface, origin_click.x, origin_click.y);
                if(pixel != SURFACE_COLOR_KEY)
                    return tile;
            }
            if(tile.reference->trait.is_inanimate && !Unit_IsExempt(tile.reference)) // BACKUP CHECK, PROBE FULL TILE BELOW BUILDING (REALLY HELPS WHEN BUILDINGS ARE BEING BUILT).
            {
                const Point cart = Overview_IsoToCart(overview, grid, cursor, false);
                if(Unit_IsPointWithinDimensions(tile.reference, cart))
                    return tile;
            }
        }
    }
    static Tile zero;
    return zero;
}

int32_t Tiles_SelectSimilar(const Tiles tiles, const Tile similar, const Color color)
{
    int32_t count = 0;
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        const Type t_a  = similar.reference->trait.type;
        const Type t_b  = tile.reference->trait.type;
        const Color c_a = similar.reference->color;
        const Color c_b = tile.reference->color;
        const bool both_villagers = Unit_IsVillager(tile.reference) && Unit_IsVillager(similar.reference);
        if((t_a == t_b || both_villagers) && c_a == c_b)
        {
            Tile_Select(tile, color);
            count++;
        }
    }
    return count;
}

int32_t Tiles_SelectWithBox(const Tiles tiles, const Rect rect, const Color color)
{
    int32_t count = 0;
    const Rect box = Rect_CorrectOrientation(rect);
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        if(tile.reference->trait.is_inanimate
        || tile.reference->trait.type == TYPE_FLAG)
            continue;
        if(Tile_IsHotspotInRect(tile, box))
        {
            Tile_Select(tile, color);
            count++;
        }
    }
    return count;
}

Tiles Tiles_Copy(const Tiles tiles)
{
    Tiles copy = tiles;
    copy.tile = UTIL_ALLOC(Tile, tiles.count);
    copy.count = tiles.count;
    for(int32_t i = 0; i < tiles.count; i++)
        copy.tile[i] = tiles.tile[i];
    return copy;
}
