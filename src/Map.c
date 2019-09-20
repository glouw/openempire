#include "Map.h"

#include "Util.h"

#include <stdlib.h>
#include <stdio.h>

static void GenerateTestZone(const Map map)
{
    for(int32_t y = 0; y < map.rows; y++)
    for(int32_t x = 0; x < map.cols; x++)
    {
        const Point point = { x, y };
        Map_SetTerrainFile(map, point, FILE_DIRT);
    }
}

Map Map_Make(const int32_t size, const Registrar terrain)
{
    const Frame frame = terrain.animation[COLOR_BLU][FILE_DIRT].frame[0];
    static Map zero;
    Map map = zero;
    map.rows = size;
    map.cols = size;
    map.file = UTIL_ALLOC(Terrain, map.rows * map.cols);
    map.tile_width = frame.width;
    map.tile_height = frame.height;
#if 1
    GenerateTestZone(map);
#endif
    return map;
}

static bool InBounds(const Map map, const Point point)
{
    return point.x < map.cols && point.x >= 0
        && point.y < map.rows && point.y >= 0;
}

Terrain Map_GetTerrainFile(const Map map, const Point point)
{
    if(!InBounds(map, point))
        return FILE_TERRAIN_NONE;
    return map.file[point.x + point.y * map.cols];
}

void Map_SetTerrainFile(const Map map, const Point point, const Terrain file)
{
    if(!InBounds(map, point))
        return;
    map.file[point.x + point.y * map.cols] = file;
}

void Map_Free(const Map map)
{
    free(map.file);
}

void Map_Edit(const Map map, const Overview overview, const Input input)
{
    if(input.key[SDL_SCANCODE_LSHIFT])
    {
        Terrain file = FILE_DIRT;
        if(input.key[SDL_SCANCODE_1]) file = FILE_GRASS;
        if(input.key[SDL_SCANCODE_2]) file = FILE_WATER;
        if(input.key[SDL_SCANCODE_3]) file = FILE_FARM;
        if(input.l)
        {
            const Point cartesian = Overview_IsoToCart(overview, input.point, false);
            if(InBounds(map, cartesian))
                Map_SetTerrainFile(map, cartesian, file);
        }
    }
}

Points Map_GetBlendBox(const Map map, const Point inner)
{
    const int32_t width = 1;
    Points points = Points_New(8);
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
    Lines lines = Lines_New(8 * render_points.count);
    for(int32_t i = 0; i < render_points.count; i++)
    {
        const Point inner = render_points.point[i];
        lines = AppendBlendLines(lines, map, inner);
    }
    return lines;
}
