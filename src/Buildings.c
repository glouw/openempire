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

Buildings Buildings_Append(Buildings builds, const Building build)
{
    if(builds.count == builds.max)
    {
        builds.max *= 2;
        Building* const temp = UTIL_REALLOC(builds.build, Building, builds.max);
        UTIL_CHECK(temp);
        builds.build = temp;
    }
    builds.build[builds.count++] = build;
    return builds;
}

void Buildings_Free(const Buildings builds)
{
    free(builds.build);
    free(builds.reference);
}
