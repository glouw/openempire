#include "Field.h"

#include "Util.h"

#include <string.h>
#include <stdbool.h>

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

static bool Size(Queue queue)
{
    return queue.end - queue.start;
}

static bool IsEmpty(Queue queue)
{
    return Size(queue) == 0;
}

static bool InBounds(const Field field, const Point point)
{
    return point.x < field.cols && point.x >= 0
        && point.y < field.rows && point.y >= 0;
}

static bool IsWalkable(const Field field, const Point point)
{
    return field.object[point.x + point.y * field.cols] == ' ';
}

static Points ToPoints(const Queue queue)
{
    Points points = Points_New(Size(queue));
    for(int32_t i = 0; i < points.max; i++)
        points = Points_Append(points, queue.point[i + queue.start]);
    return points;
}

Points SearchBreadthFirst(const Field field, const Point start, const Point goal)
{
    Queue frontier = New(8);
    frontier = Enqueue(frontier, start);
    Queue came_from = New(8);
    const Point none = { -1, -1 };
    for(int32_t i = 0; i < field.rows * field.cols; i++)
        came_from = Enqueue(came_from, none);
    while(!IsEmpty(frontier))
    {
        Point current;
        frontier = Dequeue(frontier, &current);
        if(Point_Equal(current, goal))
            break;
        const Point deltas[] = {
            { -1, +1 }, { 0, +1 }, { 1, +1 },
            { -1,  0 }, /* ---- */ { 1,  0 },
            { -1, -1 }, { 0, -1 }, { 1, -1 },
        };
        for(int32_t i = 0; i < UTIL_LEN(deltas); i++)
        {
            const Point next = Point_Add(current, deltas[i]);
            if(InBounds(field, next)
            && IsWalkable(field, next))
            {
                if(Point_Equal(came_from.point[next.x + next.y * field.cols], none))
                {
                    frontier = Enqueue(frontier, next);
                    came_from.point[next.x + next.y * field.cols] = current;
                }
            }
        }
    }
    Queue path = New(8);
    Point current = goal;
    while(!Point_Equal(current, start))
    {
        path = Enqueue(path, current);
        current = came_from.point[current.x + current.y * field.cols];
    }
    path = Enqueue(path, start);
    Free(frontier);
    Free(came_from);
    return ToPoints(path);
}
