#include "Tiles.h"

#include "Util.h"
#include "Rect.h"
#include "Surface.h"

Tiles Tiles_PrepGraphics(const Registrar graphics, const Overview overview, const Units units, const Points points)
{
    Tile* const tile = UTIL_ALLOC(Tile, units.count);
    Unit** const unit = UTIL_ALLOC(Unit*, units.count);
    UTIL_CHECK(tile);
    UTIL_CHECK(unit);
    int32_t unit_count = 0;
    for(int32_t i = points.count - 1; i >= 0; i--)
    {
        const Point point = points.point[i];
        const Stack stack = Units_GetStackCart(units, point);
        for(int32_t j = 0; j < stack.count; j++)
        {
            Unit* const ref = stack.reference[j];

            // XXX: Now that unit tile is indexed, the unit must be flagged as drawn, else the same unit from a different
            // tile will call another draw for cases where a graphics file occupies more than one tile (eg. buildings).

            const Animation animation = graphics.animation[COLOR_BLU][ref->file];

            // A little unfortunate, but the hot spots for the terrain tiles are not centered.
            // Units must therefor be forced to the terrain tile positions.

            const Point south = { 0, 1 };
            const Point shifted = Point_Add(point, south);
            tile[unit_count] = Tile_GetGraphics(overview, shifted, ref->cart_fractional, animation, ref->file);
            unit[unit_count] = ref;
            unit_count++;
        }
    }
    const Tiles tiles = { tile, unit, unit_count };
    return tiles;
}

void Tiles_Free(const Tiles tiles)
{
    free(tiles.tile);
    free(tiles.unit);
}

static int32_t CompareTileSurface(const void* a, const void* b)
{
    Tile* const aa = (Tile*) a;
    Tile* const bb = (Tile*) b;
    return aa->surface < bb->surface;
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
    qsort(tile, points.count, sizeof(*tile), CompareTileSurface);
    const Tiles tiles = { tile, NULL, points.count};
    return tiles;
}

bool Tiles_Select(const Tiles tiles, const Input input)
{
    if(input.lu)
    {
        const Point click = { input.x, input.y };
        for(int32_t i = 0; i < tiles.count; i++)
        {
            const Tile tile = tiles.tile[i];
            if(Tile_ContainsPoint(tile, click))
            {
                const Rect rect = Rect_GetFrameOutline(tile);
                const Point origin_click = Point_Sub(click, rect.a);
                if(Surface_GetPixel(tile.surface, origin_click.x, origin_click.y) != SURFACE_COLOR_KEY)
                {
                    // XXX. Must draw circle around selected unit.
                    puts("GOT EM");
                    tiles.unit[i]->selected = true;
                    Point_Print(tiles.unit[i]->cart_point);
                    return true;
                }
            }
        }
        // No units were selected.
        return false;
    }
    // Return true if nothing happened.
    return true;
}
