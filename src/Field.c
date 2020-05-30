#include "Field.h"

#include "Meap.h"
#include "Util.h"
#include "Config.h"

#include <string.h>
#include <stdbool.h>

static bool IsInBounds(const Field field, const Point point)
{
    return point.x < field.size && point.x >= 0
        && point.y < field.size && point.y >= 0;
}

char Field_Get(const Field field, const Point point)
{
    return field.object[point.x + point.y * field.size];
}

void Field_Set(const Field field, const Point point, const char ch)
{
    field.object[point.x + point.y * field.size] = ch;
}

bool Field_IsWalkable(const Field field, const Point point)
{
    return IsInBounds(field, point)
        && Field_Get(field, point) == FIELD_WALKABLE_SPACE;
}

static int32_t Heuristic(const Point a, const Point b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

Points Construct(const Field field, const Point start, const Point goal, const Points came_from)
{
    Points path = Points_Make(32);
    Point current = goal;
    while(!Point_Equal(current, start))
    {
        path = Points_Append(path, current);
        current = came_from.point[current.x + current.y * field.size];
    }
    return Points_Append(path, start);
}

Points Field_PathAStar(const Field field, const Point start, const Point goal)
{
    static Points zero;
    if(!IsInBounds(field, goal))
        return zero;
    // FOR WHATEVER REASON, IF UNIT IS STUCK IN A BUILDING,
    // DO NOT PATH, ELSE ALL THE RAM WILL BE EATEN.
    // OF COURSE, THIS STATE IN A PERFECT WORLD SHOULD BE UNREACHABLE.
    if(!IsInBounds(field, start))
        return zero;
    Meap frontier = Meap_Make();
    Meap_Insert(&frontier, 0, start);
    const Point none = { -1, -1 };
    const int32_t area = field.area;
    int32_t* const cost_so_far = UTIL_ALLOC(int32_t, area);
    const Points came_from = Points_Make(area);
    for(int32_t i = 0; i < area; i++)
    {
        came_from.point[i] = none;
        cost_so_far[i] = -1;
    }
    cost_so_far[start.x + start.y * field.size] = 0;
    for(int32_t tries = 0; frontier.size > 0; tries++)
    {
        Step current = Meap_Delete(&frontier);
        if(Point_Equal(current.point, goal))
            break;
        if(tries > CONFIG_FIELD_MAX_PATHING_TRIES)
        {
            Meap_Free(&frontier);
            Points_Free(came_from);
            free(cost_so_far);
            return zero;
        }
        const Point deltas[] = {
            { -1, +1 }, { 0, +1 }, { 1, +1 },
            { -1,  0 }, /* ---- */ { 1,  0 },
            { -1, -1 }, { 0, -1 }, { 1, -1 },
        };
        for(int32_t i = 0; i < UTIL_LEN(deltas); i++)
        {
            const int32_t new_cost = cost_so_far[current.point.x + current.point.y * field.size] + 1;
            const Point delta = deltas[i];
            const Point vert = { current.point.x + delta.x, current.point.y };
            const Point horz = { current.point.x, current.point.y + delta.y };
            const Point next = Point_Add(current.point, delta);
            if(Field_IsWalkable(field, next)
            && Field_IsWalkable(field, vert)
            && Field_IsWalkable(field, horz))
            {
                const int32_t cost = cost_so_far[next.x + next.y * field.size];
                if(cost == -1 || new_cost < cost)
                {
                    cost_so_far[next.x + next.y * field.size] = new_cost;
                    const int32_t priority = Heuristic(goal, next) + new_cost;
                    Meap_Insert(&frontier, priority, next);
                    came_from.point[next.x + next.y * field.size] = current.point;
                }
            }
        }
    }
    const Points forwards = Construct(field, start, goal, came_from);
    const Points backward = Points_Reverse(forwards);
    Points_Free(came_from);
    Meap_Free(&frontier);
    Points_Free(forwards);
    free(cost_so_far);
    return backward;
}

Field Field_Make(const int32_t size)
{
    static Field zero;
    Field field = zero;
    field.size = size;
    field.area = field.size * field.size;
    field.object = UTIL_ALLOC(char, field.area);
    return field;
}

void Field_Clear(const Field field)
{
    for(int32_t i = 0; i < field.area; i++)
        field.object[i] = '\0';
}

void Field_Free(const Field field)
{
    free(field.object);
}
