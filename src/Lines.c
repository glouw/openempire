#include "Lines.h"

#include "Util.h"

#include <assert.h>

Lines Lines_New(const int32_t max)
{
    Line* const line = UTIL_ALLOC(Line, max);
    UTIL_CHECK(line);
    const Lines lines = { line, 0, max };
    return lines;
}

Lines Lines_Append(Lines lines, const Line line)
{
    assert(lines.count < lines.max);
    lines.line[lines.count++] = line;
    return lines;
}

void Lines_Free(const Lines lines)
{
    free(lines.line);
}

static int32_t CompareByX(const void* a, const void* b)
{
    Line* const aa = (Line*) a;
    Line* const bb = (Line*) b;
    return aa->outer.x > bb->outer.x;
}

static int32_t CompareByY(const void* a, const void* b)
{
    Line* const aa = (Line*) a;
    Line* const bb = (Line*) b;
    return aa->outer.y > bb->outer.y;
}

static int32_t CompareByTileFile(const void* a, const void* b)
{
    Line* const aa = (Line*) a;
    Line* const bb = (Line*) b;
    return aa->tile_file > bb->tile_file;
}

void Lines_Sort(const Lines lines)
{
    qsort(lines.line, lines.count, sizeof(*lines.line), CompareByTileFile);
    qsort(lines.line, lines.count, sizeof(*lines.line), CompareByX);
    qsort(lines.line, lines.count, sizeof(*lines.line), CompareByY);
}

void Lines_Print(const Lines lines)
{
    for(int32_t i = 0; i < lines.count; i++)
    {
        const Line line = lines.line[i];
        printf("%4d :: %4d %4d %4d %4d\n", i, line.inner.x, line.inner.y, line.outer.x, line.outer.y);
    }
}
