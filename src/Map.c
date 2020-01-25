#include "Map.h"

#include "Rect.h"
#include "Surface.h"
#include "Config.h"
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

static void TrimWaterEdge(const Map map)
{
    for(int32_t trims = 0; trims < 10; trims++)
    {
        Points edges = Points_New(32);
        for(int32_t x = 1; x < map.size - 1; x++)
        for(int32_t y = 1; y < map.size - 1; y++)
        {
            const Point point = { x, y };
            if(Map_GetTerrainFile(map, point) == FILE_TERRAIN_WATER_SHALLOW)
            {
                const Point points[] = {
                    { x + 1, y + 0 },
                    { x + 1, y + 1 },
                    { x + 0, y + 1 },
                    { x - 1, y - 1 },
                    { x - 1, y + 0 },
                    { x - 1, y - 1 },
                    { x + 0, y - 1 },
                    { x + 1, y - 1 },
                };
                int32_t count = 0;
                for(int32_t i = 0; i < UTIL_LEN(points); i++)
                    if(Map_GetTerrainFile(map, points[i]) == FILE_TERRAIN_DIRT)
                        count++;
                if(count >= 5)
                    edges = Points_Append(edges, point);
            }
        }
        for(int32_t i = 0; i < edges.count; i++)
            Map_SetTerrainFile(map, edges.point[i], FILE_TERRAIN_DIRT);
        Points_Free(edges);
    }
}

static Terrain GetTerrainFromHeight(const int32_t height)
{
    if(height < MAP_HEIGHT_WATER_DEEP)    return FILE_TERRAIN_WATER_DEEP;
    if(height < MAP_HEIGHT_WATER_NORMAL)  return FILE_TERRAIN_WATER_NORMAL;
    if(height < MAP_HEIGHT_WATER_SHALLOW) return FILE_TERRAIN_WATER_SHALLOW;
    if(height < MAP_HEIGHT_BEACH_SAND)    return FILE_TERRAIN_BEACH_SAND;
    return FILE_TERRAIN_GRASS_A;
}

static void Create(const Map map)
{
    for(int32_t y = 0; y < map.size; y++)
    for(int32_t x = 0; x < map.size; x++)
    {
        const Point point = { x, y };
        const int32_t height = map.height[x + map.size * y];
        const Terrain file = GetTerrainFromHeight(height);
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
    const Point middle = {
        map.size / 2,
        map.size / 2,
    };
    map.middle = middle;
    map = PopulateMiniMapColors(map, terrain);
    GenerateHeight(map, map.size);
    NormalizeHeight(map);
    Create(map);
    TrimWaterEdge(map);
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
    Points points = Points_New(size);
    for(int32_t i = 0; i < size; i++)
        points = Points_Append(points, slots[i]);
    return points;
}

static bool IsAreaClear(const Map map, const Point point, const int32_t width)
{
    for(int32_t x = -width; x < width; x++)
    for(int32_t y = -width; y < width; y++)
    {
        const Point shift = { x, y };
        const Point cart = Point_Add(point, shift);
        const Terrain terrain = Map_GetTerrainFile(map, cart);
        const bool none = terrain == FILE_TERRAIN_NONE;
        if(none || !Terrain_IsWalkable(terrain)) // WATER OR SOMETHING LIKE THAT.
            return false;
    }
    return true;
}

Point Map_GetFixedSlot(const Map map, const Point slot)
{
    const int32_t step_size = 5;
    const Point dir = Point_Sub(map.middle, slot);
    const Point step = Point_Normalize(dir, step_size);
    const int32_t width = CONFIG_UNITS_STARTING_AREA_TILE_SPACE / 2;
    Point fixed = slot;
    while(!IsAreaClear(map, fixed, width))
        fixed = Point_Add(fixed, step);
    return fixed;
}
