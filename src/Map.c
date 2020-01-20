#include "Map.h"

#include "Rect.h"
#include "Surface.h"
#include "Util.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

static void Interpolate(const Map map, const Rect rect)
{
    for(int32_t x = rect.a.x; x < rect.b.x; x++)
    for(int32_t y = rect.a.y; y < rect.b.y; y++)
        map.height[y + map.size * x] = (
            map.height[rect.a.y + map.size * rect.a.x] * (rect.b.x - x) * (rect.b.y - y) +
            map.height[rect.a.y + map.size * rect.b.x] * (x - rect.a.x) * (rect.b.y - y) +
            map.height[rect.b.y + map.size * rect.a.x] * (rect.b.x - x) * (y - rect.a.y) +
            map.height[rect.b.y + map.size * rect.b.x] * (x - rect.a.x) * (y - rect.a.y)
        ) / ((rect.b.x - rect.a.x) * (rect.b.y - rect.a.y));
}

static void GenerateHeight(const Map map, const int32_t square)
{
    if(square == 2)
        return;
    const int32_t m = (square - 1) / 2;
    for(int32_t x = m; x < map.size; x += 2 * m)
    for(int32_t y = m; y < map.size; y += 2 * m)
        map.height[x + map.size * y] += Util_Rand() % (2 * square + 1) - square;
    for(int32_t x = 0; x < map.size - 1; x += m)
    for(int32_t y = 0; y < map.size - 1; y += m)
    {
        const Rect rect = {
            { x + 0, y + 0 },
            { x + m, y + m },
        };
        Interpolate(map, rect);
    }
    GenerateHeight(map, m + 1);
}

static int32_t Max(const Map map)
{
    int32_t max = INT_MIN;
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
    {
        const int32_t height = map.height[y + map.size * x];
        if(height > max)
            max = height;
    }
    return max;
}

static int32_t Min(const Map map)
{
    int32_t min = INT_MAX;
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
    {
        const int32_t height = map.height[y + map.size * x];
        if(height < min)
            min = height;
    }
    return min;
}

static void Add(const Map map, const int32_t shift)
{
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
        map.height[y + map.size * x] += shift;
}

static void NormalizeHeight(const Map map)
{
    const int32_t min = Min(map);
    const int32_t shift = abs(min);
    Add(map, shift);
    const int32_t max = Max(map);
    for(int32_t x = 0; x < map.size; x++)
    for(int32_t y = 0; y < map.size; y++)
    {
        int32_t* const height = &map.height[y + map.size * x];
        *height = (*height * MAP_HEIGHT_MAX) / max;
    }
}

static void Create(const Map map)
{
    for(int32_t y = 0; y < map.size; y++)
    for(int32_t x = 0; x < map.size; x++)
    {
        const Point point = { x, y };
        const int32_t height = map.height[x + map.size * y];
        Terrain file = FILE_TERRAIN_GRASS_A;
        if(height < MAP_HEIGHT_DIRT)          file = FILE_TERRAIN_DIRT;
        if(height < MAP_HEIGHT_WATER_SHALLOW) file = FILE_TERRAIN_WATER_SHALLOW;
        if(height < MAP_HEIGHT_WATER_NORMAL)  file = FILE_TERRAIN_WATER_NORMAL;
        if(height < MAP_HEIGHT_WATER_DEEP)    file = FILE_TERRAIN_WATER_DEEP;
        Map_SetTerrainFile(map, point, file);
    }
}

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

Map Map_Make(const int32_t power, const Registrar terrain)
{
    const Frame frame = terrain.animation[COLOR_GAIA][FILE_TERRAIN_DIRT].frame[0];
    const int32_t size = Util_Pow(2, power) + 1;
    const int32_t area = size * size;
    static Map zero;
    Map map = zero;
    map.size = size;
    map.file = UTIL_ALLOC(Terrain, area);
    map.height = UTIL_ALLOC(int32_t, area);
    map.tile_width = frame.width;
    map.tile_height = frame.height;
    map = PopulateMiniMapColors(map, terrain);
    GenerateHeight(map, map.size);
    NormalizeHeight(map);
    Create(map);
    return map;
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
    free(map.height);
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

int32_t Map_GetHeight(const Map map, const Point point)
{
    if(!InBounds(map, point))
        return 0;
    return map.height[point.x + point.y * map.size];
}
