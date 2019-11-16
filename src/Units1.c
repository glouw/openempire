// General purpose test zones.

#include "Units.h"

#include "Util.h"

static Units GenerateBattleZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    const Point none = { 0,0 };
    const int32_t depth = 5;
    for(int32_t x = 0; x < depth; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        const Graphics file = y < map.rows / 3 ? FILE_KNIGHT_IDLE : FILE_TEUTONIC_KNIGHT_IDLE;
        units = Units_Spawn(units, cart, none, grid, file, COLOR_BLU, graphics, map);
    }
    for(int32_t x = map.cols - depth; x < map.cols; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        units = Units_Spawn(units, cart, none, grid, FILE_KNIGHT_IDLE, COLOR_RED, graphics, map);
    }
    const Point cart = { map.cols / 2, map.rows / 2 };
    units = Units_Spawn(units, cart, none, grid, FILE_KNIGHT_IDLE, COLOR_BLU, graphics, map);
    const Field field = Units_Field(units, map);
    for(int32_t i = 0; i < units.count; i++)
    {
        static Point zero;
        Unit* const unit = &units.unit[i];
        if(unit->color == COLOR_BLU)
        {
            const Point point = { map.cols - 1, map.rows / 2 };
            Unit_FindPath(unit, point, zero, field);
        }
        else
        {
            const Point point = { 0, map.rows / 2 };
            Unit_FindPath(unit, point, zero, field);
        }
    }
    Field_Free(field);
    return units;
}

static Units GenerateInanimateZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    const Point none = { 0,0 };
    const Point a = { grid.cols / 2, grid.cols / 2 };
    const Point b = { grid.cols / 2 - 4, grid.cols / 2 };
    const Point c = { grid.cols / 2 + 2, grid.cols / 2 + 3 };
    const Point d = { grid.cols / 2 + 6, grid.cols / 2 + 6 };
    const Point e = { grid.cols / 2 - 8, grid.cols / 2 - 8 };
    const Point f = { grid.cols / 2 + 9, grid.cols / 2 + 9 };
    const Point g = { grid.cols / 2 + 10, grid.cols / 2 + 16 };
    units = Units_Spawn(units, a, none, grid, FILE_FEUDAL_BARRACKS_NORTH_EUROPEAN, COLOR_BLU, graphics, map);
    units = Units_Spawn(units, c, none, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_RED, graphics, map);
    units = Units_Spawn(units, d, none, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_BLU, graphics, map);
    units = Units_Spawn(units, e, none, grid, FILE_WONDER_BRITONS, COLOR_BLU, graphics, map);
    units = Units_Spawn(units, f, none, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_BLU, graphics, map);
    units = Units_SpawnWithShadow(units, g, grid, FILE_NORTH_EUROPEAN_CASTLE, COLOR_BLU, graphics, FILE_NORTH_EUROPEAN_CASTLE_SHADOW, map);
    for(int32_t i = 0; i < 300; i++)
        units = Units_Spawn(units, b, none, grid, FILE_TEUTONIC_KNIGHT_IDLE, COLOR_BLU, graphics, map);
    for(int32_t j = 0; j < 10; j++)
    for(int32_t i = 0; i < 10; i++)
    {
        const Point h = { i, j };
        units = Units_SpawnWithShadow(units, h, grid, FILE_FOREST_TREE, COLOR_GRY, graphics, FILE_FOREST_TREE_SHADOW, map);
    }
    units = Units_Spawn(units, b, none, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics, map);
    return units;
}

static Units GenerateTreeZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    for(int32_t j = 0; j < units.rows; j++)
    for(int32_t i = 0; i < units.cols; i++)
    {
        const Point a = { i, j };
        units = Units_SpawnWithShadow(units, a, grid, FILE_FOREST_TREE, COLOR_GRY, graphics, FILE_FOREST_TREE_SHADOW, map);
    }
    return units;
}

static Units GenerateGameZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    const Point none = { 0,0 };
    const Point middle = { grid.cols / 2, grid.cols / 2 };
    units = Units_SpawnTownCenter(units, middle, grid, COLOR_BLU, graphics, map);
    for(int32_t i = 0; i < 5; i++)
    {
        const Point aa = { -2, +3 };
        const Point bb = { -9, -9 };
        const Point cc = {  9,  9 };
        const Point a = Point_Add(middle, aa);
        const Point b = Point_Add(middle, bb);
        const Point c = Point_Add(middle, cc);
        units = Units_Spawn(units, a, none, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics, map);
        units = Units_Spawn(units, b, none, grid, FILE_MALE_VILLAGER_IDLE, COLOR_RED, graphics, map);
        units = Units_Spawn(units, c, none, grid, FILE_MILITIA_IDLE, COLOR_BLU, graphics, map);
    }
    const Point cc = { -3, -3 };
    const Point dd = {  3,  3 };
    const Point ee = {  3, -3 };
    const Point ff = { -6,  3 };
    const Point c = Point_Add(middle, cc);
    const Point d = Point_Add(middle, dd);
    const Point e = Point_Add(middle, ee);
    const Point f = Point_Add(middle, ff);
    units = Units_SpawnWithShadow(units, c, grid, FILE_FOREST_TREE, COLOR_GRY, graphics, FILE_FOREST_TREE_SHADOW, map);
    units = Units_Spawn(units, d, none, grid, FILE_STONE_MINE, COLOR_GRY, graphics, map);
    units = Units_Spawn(units, e, none, grid, FILE_BERRY_BUSH, COLOR_GRY, graphics, map);
    units = Units_Spawn(units, f, none, grid, FILE_GOLD_MINE, COLOR_GRY, graphics, map);
    return units;
}

Units Units_GenerateTestZone(const Units units, const Map map, const Grid grid, const Registrar graphics)
{
    switch(3)
    {
    default:
    case 0: return GenerateBattleZone(units, map, grid, graphics);
    case 1: return GenerateInanimateZone(units, map, grid, graphics);
    case 2: return GenerateTreeZone(units, map, grid, graphics);
    case 3: return GenerateGameZone(units, map, grid, graphics);
    }
}
