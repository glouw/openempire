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
    for(int32_t i = 0; i < 500; i++)
    {
        const Point point = {
            Util_Rand() % map.cols,
            Util_Rand() % map.rows,
        };
        switch(Util_Rand() % 3)
        {
        default:
        case 0:
            Map_SetTerrainFile(map, point, FILE_GRASS);
            break;
        case 1:
            Map_SetTerrainFile(map, point, FILE_WATER);
            break;
        case 2:
            Map_SetTerrainFile(map, point, FILE_FARM);
            break;
        }
    }
}

Map Map_Make(const int32_t rows, const int32_t cols, const Registrar terrain)
{
    const Frame frame = terrain.animation[COLOR_BLU][FILE_DIRT].frame[0];
    static Map zero;
    Map map = zero;
    map.rows = rows;
    map.cols = cols;
    map.file = UTIL_ALLOC(Terrain, rows * cols);
    map.tile_width = frame.width;
    map.tile_height = frame.height;
    GenerateTestZone(map);
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
    Terrain file = FILE_DIRT;
    if(input.key[SDL_SCANCODE_1]) file = FILE_GRASS;
    if(input.key[SDL_SCANCODE_2]) file = FILE_WATER;
    if(input.key[SDL_SCANCODE_3]) file = FILE_FARM;
    if(input.l)
    {
        const Point point = { input.x, input.y };
        const Point cartesian = Overview_IsoToCart(overview, point, false);
        if(InBounds(map, cartesian))
            Map_SetTerrainFile(map, cartesian, file);
    }
}

// Before a point is appended, ensure that:
//   1. A tile is not out of bounds.
//   2. The priority of the inner tile is greater than that of the outer tile.

Points Map_GetBlendBox(const Map map, const Point inner)
{
    Points points = Points_New(8);
    const int32_t inner_tile = Map_GetTerrainFile(map, inner);
    for(int32_t dx = -1; dx <= 1; dx++)
    for(int32_t dy = -1; dy <= 1; dy++)
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
