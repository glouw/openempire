#include "Field.h"

#include "Util.h"

#include <string.h>
#include <stdbool.h>

// Fields are one long string representing walkable tiles on the map.
// A walkable tile, like grass, dirt, or snow, are marked by a space character.
// An unwalkable tile is any other character.

static const char WALKABLE_SPACE = ' ';
static const char OBSTRUCT_SPACE = '#';

typedef struct
{
    Point* point;
    int32_t start;
    int32_t end;
    int32_t max;
}
Queue;

static Queue New(const int32_t max)
{
    Point* point = UTIL_ALLOC(Point, max);
    UTIL_CHECK(point);
    Queue queue = { point, 0, 0, max };
    return queue;
}

static Queue Enqueue(Queue queue, const Point point)
{
    if(queue.end == queue.max)
    {
        queue.max *= 2;
        Point* const temp = UTIL_REALLOC(queue.point, Point, queue.max);
        UTIL_CHECK(temp);
        queue.point = temp;
    }
    queue.point[queue.end++] = point;
    return queue;
}

static Queue Dequeue(Queue queue, Point* point)
{
    *point = queue.point[queue.start++];
    return queue;
}

static void Free(Queue queue)
{
    free(queue.point);
}

static int32_t Size(Queue queue)
{
    return queue.end - queue.start;
}

static bool IsEmpty(Queue queue)
{
    return Size(queue) == 0;
}

static bool IsInBounds(const Field field, const Point point)
{
    return point.x < field.cols && point.x >= 0
        && point.y < field.rows && point.y >= 0;
}

static char Get(const Field field, const Point point)
{
    return field.object[point.x + point.y * field.cols];
}

static void Set(const Field field, const Point point, const char ch)
{
    field.object[point.x + point.y * field.cols] = ch;
}

static bool IsWalkable(const Field field, const Point point)
{
    return Get(field, point) == WALKABLE_SPACE;
}

static Points ToPoints(const Queue queue)
{
    Points points = Points_New(Size(queue));
    for(int32_t i = 0; i < points.max; i++)
        points = Points_Append(points, queue.point[i + queue.start]);
    return points;
}

Field Field_New(const Map map, const Units units)
{
    static Field zero;
    Field field = zero;
    field.rows = map.rows;
    field.cols = map.cols;
    field.object = UTIL_ALLOC(char, field.rows * field.cols);
    for(int32_t row = 0; row < field.rows; row++)
    for(int32_t col = 0; col < field.cols; col++)
    {
        const Point point = { col, row };
        if(Units_CanWalk(units, map, point))
            Set(field, point, WALKABLE_SPACE);
        else
            Set(field, point, OBSTRUCT_SPACE);
    }
    return field;
}

static Point AccessQueue(const Field field, const Queue queue, const Point point)
{
    return queue.point[point.x + point.y * field.cols];
}

static void ModifyQueue(const Field field, const Queue queue, const Point point, const Point mod)
{
    queue.point[point.x + point.y * field.cols] = mod;
}

Points Field_SearchBreadthFirst(const Field field, const Point start, const Point goal) // XXX. THIS can be ASTAR and it will be so much faster!
{
    const Point deltas[] = {
        { -1, +1 }, { 0, +1 }, { 1, +1 },
        { -1,  0 },            { 1,  0 },
        { -1, -1 }, { 0, -1 }, { 1, -1 },
    };
    const int32_t dirs = UTIL_LEN(deltas);

    Queue frontier = New(dirs);
    Queue came_from = New(dirs);

    // Setup.

    frontier = Enqueue(frontier, start);
    const Point none = { -1, -1 };
    for(int32_t i = 0; i < field.rows * field.cols; i++)
        came_from = Enqueue(came_from, none);

    // Breadth First Search.

    while(!IsEmpty(frontier))
    {
        Point current;
        frontier = Dequeue(frontier, &current);
        if(Point_Equal(current, goal))
            break;
        for(int32_t i = 0; i < dirs; i++)
        {
            const Point next = Point_Add(current, deltas[i]);
            if(IsInBounds(field, next)
            && IsWalkable(field, next))
            {
                if(Point_Equal(AccessQueue(field, came_from, next), none))
                {
                    frontier = Enqueue(frontier, next);
                    ModifyQueue(field, came_from, next, current);
                }
            }
        }
    }

    // Construct path.

    Queue path = New(dirs);
    Point current = goal;
    while(!Point_Equal(current, start))
    {
        path = Enqueue(path, current);
        current = AccessQueue(field, came_from, current);
    }
    path = Enqueue(path, start);
    Free(frontier);
    Free(came_from);

    // Export points to prevent exporting Queue type.
    // Points are reversed as queue is reversed.

    const Points points = ToPoints(path);
    const Points reversed = Points_Reverse(points);
    Points_Free(points);

    return reversed;
}

void Field_Free(const Field field)
{
    free(field.object);
}
