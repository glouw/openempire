#include "Map.h"

#include "Rect.h"
#include "Surface.h"
#include "Config.h"
#include "Util.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

static Map PopulateMiniMapColors(Map map, const Registrar terrain)
{
    for(int32_t i = 0; i < TERRAIN_COUNT; i++)
    {
        const Animation animation = terrain.animation[COLOR_GAIA][i];
        SDL_Surface* const surface = animation.surface[0];
        uint32_t* pixels = (uint32_t*) surface->pixels;
        const int32_t size = surface->w * surface->h;
        int64_t r = 0x0;
        int64_t g = 0x0;
        int64_t b = 0x0;
        for(int j = 0; j < size; j++)
        {
            r += (pixels[j] >> SURFACE_R_SHIFT) & 0xFF;
            g += (pixels[j] >> SURFACE_G_SHIFT) & 0xFF;
            b += (pixels[j] >> SURFACE_B_SHIFT) & 0xFF;
        }
        r /= size;
        g /= size;
        b /= size;
        map.color[i] = (Color) (
            (r << SURFACE_R_SHIFT) |
            (g << SURFACE_G_SHIFT) |
            (b << SURFACE_B_SHIFT)
        );
    }
    return map;
}

static void LayDirt(const Map map)
{
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
    {
        const Point point = { x, y };
        Map_SetTerrainFile(map, point, FILE_TERRAIN_DIRT);
    }
}

static void SprinkleGrass(const Map map)
{
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
    {
        if(Util_FlipCoin()
        && Util_FlipCoin()
        && Util_FlipCoin())
        {
            const Point point = { x, y };
            Map_SetTerrainFile(map, point, FILE_TERRAIN_GRASS_A);
        }
    }
}

Map Map_Make(const int32_t size, const Registrar terrain)
{
    const Frame frame = terrain.animation[COLOR_GAIA][FILE_TERRAIN_DIRT].frame[0];
    const int32_t area = size * size;
    static Map zero;
    Map map = zero;
    map.size = size;
    map.file = UTIL_ALLOC(Terrain, area);
    map.tile_width = frame.width;
    map.tile_height = frame.height;
    const Point middle = {
        map.size / 2,
        map.size / 2,
    };
    map.middle = middle;
    LayDirt(map);
    SprinkleGrass(map);
    return PopulateMiniMapColors(map, terrain);
}

static bool InBounds(const Map map, const Point point)
{
    return point.x < map.size && point.x >= 0
        && point.y < map.size && point.y >= 0;
}

Terrain Map_GetTerrainFile(const Map map, const Point point)
{
    if(!InBounds(map, point))
        return FILE_TERRAIN_NONE;
    return map.file[point.x + point.y * map.size];
}

void Map_SetTerrainFile(const Map map, const Point point, const Terrain file)
{
    if(!InBounds(map, point))
        return;
    map.file[point.x + point.y * map.size] = file;
}

void Map_Free(const Map map)
{
    free(map.file);
}

Points Map_GetBlendBox(const Map map, const Point inner)
{
    const int32_t width = 1;
    Points points = Points_Make(8);
    const int32_t inner_tile = Map_GetTerrainFile(map, inner);
    for(int32_t dx = -width; dx <= width; dx++)
    for(int32_t dy = -width; dy <= width; dy++)
    {
        const Point delta = { dx, dy };
        const Point outer = Point_Add(inner, delta);
        if(!Point_Equal(inner, outer))
        {
            const int32_t outer_tile = Map_GetTerrainFile(map, outer);
            if(outer_tile != FILE_TERRAIN_NONE
            && inner_tile > outer_tile)
                points = Points_Append(points, outer);
        }
    }
    return points;
}

static Lines AppendBlendLines(Lines lines, const Map map, const Point inner)
{
    const Points box = Map_GetBlendBox(map, inner);
    for(int32_t j = 0; j < box.count; j++)
    {
        const Terrain file = Map_GetTerrainFile(map, inner);
        const Point outer = box.point[j];
        const Line line = { inner, outer, file };
        lines = Lines_Append(lines, line);
    }
    Points_Free(box);
    return lines;
}

Lines Map_GetBlendLines(const Map map, const Points render_points)
{
    Lines lines = Lines_Make(8 * render_points.count);
    for(int32_t i = 0; i < render_points.count; i++)
    {
        const Point inner = render_points.point[i];
        lines = AppendBlendLines(lines, map, inner);
    }
    return lines;
}

Points Map_GetSlots(const Map map)
{
    const int32_t from_edge = CONFIG_UNITS_STARTING_AREA_TILE_SPACE;
    const int32_t dx = map.middle.x - from_edge;
    const int32_t dy = map.middle.y - from_edge;
    const Point slots[] = {
        { map.middle.x + dx, map.middle.y      }, // E.
        { map.middle.x + dx, map.middle.y + dy }, // SE.
        { map.middle.x,      map.middle.y + dy }, // S.
        { map.middle.x - dx, map.middle.y + dy }, // SW.
        { map.middle.x - dx, map.middle.y      }, // W.
        { map.middle.x - dx, map.middle.y - dy }, // NW
        { map.middle.x,      map.middle.y - dy }, // N.
        { map.middle.x + dx, map.middle.y - dy }, // NE.
    };
    const int32_t size = UTIL_LEN(slots);
    Points points = Points_Make(size);
    for(int32_t i = 0; i < size; i++)
        points = Points_Append(points, slots[i]);
    return points;
}
