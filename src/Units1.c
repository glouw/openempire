// General purpose test zones.

#include "Units.h"

#include "Util.h"

static Units GenerateBattleZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    const int32_t depth = 5;
    for(int32_t x = 0; x < depth; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        const Graphics file = y < map.rows / 3 ? FILE_KNIGHT_IDLE : FILE_TEUTONIC_KNIGHT_IDLE;
        units = Units_Spawn(units, cart, grid, file, COLOR_BLU, graphics);
    }
    for(int32_t x = map.cols - depth; x < map.cols; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        units = Units_Spawn(units, cart, grid, FILE_KNIGHT_IDLE, COLOR_RED, graphics);
    }
    const Point cart = { map.cols / 2, map.rows / 2 };
    units = Units_Spawn(units, cart, grid, FILE_KNIGHT_IDLE, COLOR_BLU, graphics);
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

static Units GenerateVillieZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { units.cols / 2 + 0, units.rows / 2 + 0 };
    const Point b = { units.cols / 2 + 0, units.rows / 2 + 1 };
    units = Units_Spawn(units, a, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics);
    units = Units_Spawn(units, b, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics);
    return units;
}

static Units GenerateBerryZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { grid.cols / 2 + 0, grid.cols / 2 - 1};
    const Point b = { grid.cols / 2 - 1, grid.cols / 2 + 1};
    const Point c = { grid.cols / 2 - 0, grid.cols / 2 + 1};
    const Point d = { grid.cols / 2 - 0, grid.cols / 2 - 2};
    const Point e = { grid.cols / 2 - 0, grid.cols / 2 - 4};
    const Point f = { grid.cols / 2 - 2, grid.cols / 2 - 4};
    units = Units_Spawn(units, a, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    units = Units_Spawn(units, b, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    units = Units_Spawn(units, c, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    units = Units_Spawn(units, d, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    units = Units_Spawn(units, e, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    units = Units_Spawn(units, f, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics);
    return units;
}

static Units GenerateRandomZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { grid.cols / 2 + 0, grid.cols / 2 - 1};
    const Point b = { grid.cols / 2 - 1, grid.cols / 2 + 1};
    units = Units_Spawn(units, a, grid, FILE_RIGHT_CLICK_RED_ARROWS, COLOR_BLU, graphics);
    units = Units_Spawn(units, b, grid, FILE_WAYPOINT_FLAG, COLOR_BLU, graphics);
    return units;
}

static Units GenerateBuildingZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { grid.cols / 2, grid.cols / 2 }; // XXX. THIS BUILDING POINT needs to be bottom left corner of building.
    const Point b = { grid.cols / 2 - 4, grid.cols / 2 };
    const Point c = { grid.cols / 2 + 2, grid.cols / 2 + 3 };
    const Point d = { grid.cols / 2 + 6, grid.cols / 2 + 6 };
    const Point e = { grid.cols / 2 - 8, grid.cols / 2 - 8 };
    const Point f = { grid.cols / 2 + 9, grid.cols / 2 + 9 };
    const Point g = { grid.cols / 2 + 10, grid.cols / 2 + 16 };
    units = Units_Spawn(units, a, grid, FILE_FEUDAL_BARRACKS_NORTH_EUROPEAN, COLOR_BLU, graphics);
    units = Units_Spawn(units, c, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_RED, graphics);
    units = Units_Spawn(units, d, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_BLU, graphics);
    units = Units_Spawn(units, e, grid, FILE_WONDER_BRITONS, COLOR_BLU, graphics);
    units = Units_Spawn(units, f, grid, FILE_FEUDAL_HOUSE_NORTH_EUROPEAN, COLOR_BLU, graphics);
    units = Units_SpawnWithShadow(units, g, grid, FILE_NORTH_EUROPEAN_CASTLE, COLOR_BLU, graphics, FILE_NORTH_EUROPEAN_CASTLE_SHADOW);
    for(int32_t i = 0; i < 300; i++)
        units = Units_Spawn(units, b, grid, FILE_TEUTONIC_KNIGHT_IDLE, COLOR_BLU, graphics);
    for(int32_t j = 0; j < 10; j++)
    for(int32_t i = 0; i < 10; i++)
    {
        const Point h = { i, j };
        units = Units_SpawnWithShadow(units, h, grid, FILE_FOREST_TREE, COLOR_RED, graphics, FILE_FOREST_TREE_SHADOW);
    }
    units = Units_Spawn(units, b, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics);
    return units;
}

static Units GenerateTreeZone(Units units, const Grid grid, const Registrar graphics)
{
    for(int32_t j = 0; j < units.rows; j++)
    for(int32_t i = 0; i < units.cols; i++)
    {
        const Point a = { i, j };
        units = Units_SpawnWithShadow(units, a, grid, FILE_FOREST_TREE, COLOR_BLU, graphics, FILE_FOREST_TREE_SHADOW);
    }
    return units;
}

Units Units_GenerateTestZone(const Units units, const Map map, const Grid grid, const Registrar graphics)
{
    switch(0)
    {
        default:
        case 0: return GenerateBattleZone(units, map, grid, graphics);
        case 1: return GenerateBerryZone(units, grid, graphics);
        case 2: return GenerateVillieZone(units, grid, graphics);
        case 3: return GenerateRandomZone(units, grid, graphics);
        case 4: return GenerateBuildingZone(units, grid, graphics);
        case 5: return GenerateTreeZone(units, grid, graphics);
    }
}
