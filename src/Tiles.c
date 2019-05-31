#include "Tiles.h"

#include "Util.h"
#include "Rect.h"
#include "Surface.h"

static int32_t CompareByCartY(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    return aa->reference->cart.y < bb->reference->cart.y;
}

static void SortByCartY(const Tiles tiles)
{
    qsort(tiles.tile, tiles.count, sizeof(*tiles.tile), CompareByCartY);
}

Tiles Tiles_PrepGraphics(const Registrar graphics, const Overview overview, const Units units, const Points points)
{
    Tile* const tile = UTIL_ALLOC(Tile, units.count);
    UTIL_CHECK(tile);
    int32_t unit_count = 0;
    for(int32_t i = 0; i < points.count; i++)
    {
        const Point point = points.point[i];
        const Stack stack = Units_GetStackCart(units, point);
        for(int32_t j = 0; j < stack.count; j++)
        {
            Unit* const ref = stack.reference[j];

            // XXX: Now that unit tile is indexed, the unit must be flagged as drawn, else the same unit from a different
            // tile will call another draw for cases where a graphics file occupies more than one tile (eg. buildings).

            const Animation animation = graphics.animation[COLOR_BLU][ref->file];

            tile[unit_count] = Tile_GetGraphics(overview, point, ref->cart_grid_offset, animation, ref->file);
            tile[unit_count].reference = ref;
            unit_count++;
        }
    }
    const Tiles tiles = { tile, unit_count };
    SortByCartY(tiles);
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
    qsort(tiles.tile, tiles.count, sizeof(*tiles.tile), CompareByTileSurface);
}

Tiles Tiles_PrepTerrain(const Registrar terrain, const Map map, const Overview overview, const Points points)
{
    Tile* const tile = UTIL_ALLOC(Tile, points.count);
    UTIL_CHECK(tile);
    for(int32_t i = 0; i < points.count; i++)
    {
        const Point point = points.point[i];
        const Terrain tile_file = Map_GetTerrainFile(map, point);
        if(tile_file != FILE_TERRAIN_NONE)
        {
            const Animation animation = terrain.animation[COLOR_BLU][tile_file];
            tile[i] = Tile_GetTerrain(overview, point, animation, tile_file);
        }
    }
    const Tiles tiles = { tile, points.count };
    SortByTileSurface(tiles);
    return tiles;
}

void Tiles_SelectOne(const Tiles tiles, const Point click)
{
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        if(Tile_ContainsPoint(tile, click))
        {
            const Rect rect = Tile_GetFrameOutline(tile);
            const Point origin_click = Point_Sub(click, rect.a);
            if(Surface_GetPixel(tile.surface, origin_click.x, origin_click.y) != SURFACE_COLOR_KEY)
            {
                // XXX. Must draw circle around selected unit.
                puts("GOT EM");
                tiles.tile[i].reference->selected = true;
                Point_Print(tiles.tile[i].reference->cart);
                break;
            }
        }
    }
}

void Tiles_SelectMany(const Tiles tiles, const Rect rect)
{
    const Rect box = Rect_CorrectOrientation(rect);
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        if(Tile_IsHotspotInRect(tile, box))
        {
            // XXX. Must draw circle around selected unit.
            puts("GOT EM");
            tiles.tile[i].reference->selected = true;
            Point_Print(tiles.tile[i].reference->cart);
        }
    }
}
