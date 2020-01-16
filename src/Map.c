#include "Map.h"

#include "Rect.h"
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
    for(int32_t i = m; i < map.size; i += 2 * m)
    for(int32_t j = m; j < map.size; j += 2 * m)
        map.height[j + map.size * i] += Util_Rand() % (2 * square + 1) - square;
    for(int32_t i = 0; i < map.size - 1; i += m)
    for(int32_t j = 0; j < map.size - 1; j += m)
    {
        const Rect rect = {
            { i + 0, j + 0 },
            { i + m, j + m },
        };
        Interpolate(map, rect);
    }
    GenerateHeight(map, m + 1);
}

static int32_t Max(const Map map)
{
    int32_t max = INT_MIN;
    for(int32_t i = 0; i < map.size; i++)
    for(int32_t j = 0; j < map.size; j++)
        if(map.height[j + map.size * i] > max)
            max = map.height[j + map.size * i];
    return max;
}

static int32_t Min(const Map map)
{
    int32_t min = INT_MAX;
    for(int32_t i = 0; i < map.size; i++)
    for(int32_t j = 0; j < map.size; j++)
        if(map.height[j + map.size * i] < min)
            min = map.height[j + map.size * i];
    return min;
}

static void Add(const Map map, const int32_t shift)
{
    for(int32_t i = 0; i < map.size; i++)
    for(int32_t j = 0; j < map.size; j++)
        map.height[j + map.size * i] += shift;
}

static void NormalizeHeight(const Map map)
{
    const int32_t min = Min(map);
    const int32_t shift = abs(min);
    Add(map, shift);
    const int32_t max = Max(map);
    for(int32_t i = 0; i < map.size; i++)
    for(int32_t j = 0; j < map.size; j++)
        map.height[j + map.size * i] = (0xFF * map.height[j + map.size * i]) / max;
}

#define GRASS 0x2A
#define SEA   0x5F

static void Create(const Map map)
{
    for(int32_t y = 0; y < map.size; y++)
    for(int32_t x = 0; x < map.size; x++)
    {
        const Point point = { x, y };
        const int32_t height = map.height[y + map.size * x];
        Terrain file = FILE_DIRT;
        if(height < GRASS)
            file = FILE_GRASS;
        Map_SetTerrainFile(map, point, file);
    }
}

static void Draw(const Map map)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(map.size, map.size, 0, &window, &renderer);
    for(int i = 0; i < map.size; i++)
    for(int j = 0; j < map.size; j++)
    {
        const int a = 0x2A; // Sea floor.
        const int b = 0x5F; // Sea level.
        const int p = map.height[j + map.size * i];
        p < a ?
            SDL_SetRenderDrawColor(renderer, 0, 0, a, 0): // Sea floor.
            p < b ?
            SDL_SetRenderDrawColor(renderer, 0, 0, p, 0): // Sea.
            SDL_SetRenderDrawColor(renderer, p, p, p, 0); // Ice and snow.
        SDL_RenderDrawPoint(renderer, i, j);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

Map Map_Make(const int32_t power, const Registrar terrain)
{
    const Frame frame = terrain.animation[COLOR_GAIA][FILE_DIRT].frame[0];
    const int32_t size = Util_Pow(2, power) + 1;
    const int32_t area = size * size;
    static Map zero;
    Map map = zero;
    map.size = size;
    map.file = UTIL_ALLOC(Terrain, area);
    map.height = UTIL_ALLOC(int32_t, area);
    map.tile_width = frame.width;
    map.tile_height = frame.height;
    GenerateHeight(map, map.size);
    NormalizeHeight(map);
    Draw(map);
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
