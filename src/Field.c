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

static bool IsWalkable(const Field field, const Point point)
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
    Points path = Points_New(32);
    Point current = goal;
    while(!Point_Equal(current, start))
    {
        path = Points_Append(path, current);
        current = came_from.point[current.x + current.y * field.size];
    }
    return Points_Append(path, start);
}

Points Field_PathGreedyBest(const Field field, const Point start, const Point goal) // XXX: MAY GET STUCK IN PLACE IF GREEDY BEST AS THE PATH FINDER RUNS EVERY HALF A SECOND OR SO (TWO BEST PATHS CAN BE FOUND).
{
    Meap frontier = Meap_Init();
    Meap_Insert(&frontier, 0, start);
    Points came_from = Points_New(32);
    const Point none = { -1, -1 };
    for(int32_t i = 0; i < field.size * field.size; i++)
        came_from = Points_Append(came_from, none);
    for(int32_t tries = 0; frontier.size > 0; tries++)
    {
        Step current = Meap_Delete(&frontier);
        // EARLY EXIT - GOAL REACHED.
        if(Point_Equal(current.point, goal))
            break;
        // EARLY EXIT - IMPOSSIBLE GOAL.
        if(tries > CONFIG_FIELD_MAX_PATHING_TRIES)
        {
            static Points zero;
            return zero;
        }
        const Point deltas[] = {
            { -1, +1 }, { 0, +1 }, { 1, +1 },
            { -1,  0 }, /* ---- */ { 1,  0 },
            { -1, -1 }, { 0, -1 }, { 1, -1 },
        };
        for(int32_t i = 0; i < UTIL_LEN(deltas); i++)
        {
            const Point delta = deltas[i];
            const Point vert = { current.point.x + delta.x, current.point.y };
            const Point horz = { current.point.x, current.point.y + delta.y };
            const Point next = Point_Add(current.point, delta);
            // CHECK ALL THREE SO CORNERS ARE NOT CUT WITH BUILDINGS.
            if(IsWalkable(field, next)
            && IsWalkable(field, vert)
            && IsWalkable(field, horz))
            {
                if(Point_Equal(came_from.point[next.x + next.y * field.size], none))
                {
                    const int32_t priority = Heuristic(goal, next);
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
    return backward;
}

void Field_Free(const Field field)
{
    free(field.object);
}
