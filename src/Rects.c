#include "Rects.h"

#include "Util.h"

Rects Rects_Make(const int32_t count)
{
    static Rects zero;
    Rects rects = zero;
    rects.rect = UTIL_ALLOC(Rect, count);
    rects.count = count;
    return rects;
}

void Rects_Free(const Rects rects)
{
    free(rects.rect);
}
