#include "Buildings.h"

#include "Util.h"

static Buildings GenerateTestZone(Buildings builds)
{
    const Point point = {
        builds.cols / 2,
        builds.rows / 2,
    };
    return Buildings_Append(builds, Building_Make(point, FILE_FEUDAL_BARRACKS_NORTH_EUROPEAN, COLOR_BLU));
}

Buildings Buildings_New(const Grid grid)
{
    const int32_t max = 64;
    const int32_t area = grid.rows * grid.cols;
    Building* const build = UTIL_ALLOC(Building, max);
    Building** const reference = UTIL_ALLOC(Building*, area);
    UTIL_CHECK(build);
    UTIL_CHECK(reference);
    static Buildings zero;
    Buildings builds = zero;
    builds.build = build;
    builds.reference = reference;
    builds.max = max;
    builds.rows = grid.rows;
    builds.cols = grid.cols;
    return GenerateTestZone(builds);
}

static bool OutOfBounds(const Buildings builds, const Point point)
{
    return point.x < 0
        || point.y < 0
        || point.x >= builds.cols
        || point.y >= builds.rows;
}

static bool IsFreeSpace(const Buildings builds, const Building build)
{
    for(int32_t y = 0; y < build.trait.dimensions.y; y++)
    for(int32_t x = 0; x < build.trait.dimensions.x; x++)
        if(builds.reference[x + y * builds.cols] != NULL)
            return false;
    return true;
}

static void Place(const Buildings builds, Building* const ref)
{
    for(int32_t y = 0; y < ref->trait.dimensions.y; y++)
    for(int32_t x = 0; x < ref->trait.dimensions.x; x++)
        builds.reference[x + y * builds.cols] = ref;
}

Buildings Buildings_Append(Buildings builds, const Building build)
{
    if(!OutOfBounds(builds, build.cart)
    && IsFreeSpace(builds, build))
    {
        if(builds.count == builds.max)
        {
            builds.max *= 2;
            Building* const temp = UTIL_REALLOC(builds.build, Building, builds.max);
            UTIL_CHECK(temp);
            builds.build = temp;
        }
        builds.build[builds.count] = build;
        Building* const ref = &builds.build[builds.count];
        Place(builds, ref);
        builds.count++;
    }
    return builds;
}

void Buildings_Free(const Buildings builds)
{
    free(builds.build);
    free(builds.reference);
}
