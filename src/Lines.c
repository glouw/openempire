#include "Lines.h"

#include "Util.h"

#include <assert.h>

Lines Lines_Make(const int32_t max)
{
    Line* const line = UTIL_ALLOC(Line, max);
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
    return aa->file > bb->file;
}

void Lines_Sort(const Lines lines)
{
    UTIL_SORT(lines.line, lines.count, CompareByTileFile);
    UTIL_SORT(lines.line, lines.count, CompareByX);
    UTIL_SORT(lines.line, lines.count, CompareByY);
}

void Lines_Print(const Lines lines)
{
    for(int32_t i = 0; i < lines.count; i++)
    {
        const Line line = lines.line[i];
        fprintf(stderr, "%4d :: %4d %4d %4d %4d\n", i, line.inner.x, line.inner.y, line.outer.x, line.outer.y);
    }
}
