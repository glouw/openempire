#include "Meap.h"

#include "Util.h"

static void Swap(Step* const a, Step* const b)
{
    const Step temp = *a;
    *a = *b;
    *b = temp;
}

static void Heapify(Meap* const meap, const int32_t index)
{
    const int32_t l = 2 * index + 1;
    const int32_t r = 2 * index + 2;
    int32_t smallest = l < meap->size && meap->step[l].prio < meap->step[index].prio ? l : index;
    if(r < meap->size && meap->step[r].prio < meap->step[smallest].prio)
        smallest = r;
    if(smallest != index)
    {
        Step* const a = &meap->step[index];
        Step* const b = &meap->step[smallest];
        Swap(a, b);
        Heapify(meap, smallest);
    }
}

Meap Meap_Make(void)
{
    static Meap zero;
    Meap meap = zero;
    meap.max = 32;
    meap.step = UTIL_ALLOC(Step, meap.max);
    return meap;
}

static void Grow(Meap* const meap)
{
    if(meap->size == meap->max)
    {
        meap->max *= 2;
        Step* const step = UTIL_REALLOC(meap->step, Step, meap->max);
        meap->step = step;
    }
}

void Meap_Insert(Meap* const meap, const int32_t prio, const Point point)
{
    Grow(meap);
    const Step step = { point, prio };
    int32_t index = meap->size++;
    while(index > 0)
    {
        const int32_t p = (index - 1) / 2;
        if(step.prio > meap->step[p].prio)
            break;
        meap->step[index] = meap->step[p];
        index = p;
    }
    meap->step[index] = step;
}

Step Meap_Delete(Meap* const meap)
{
    if(meap->size)
    {
        const int32_t index = 0;
        const Step step = meap->step[index];
        meap->step[index] = meap->step[--meap->size];
        Heapify(meap, index);
        return step;
    }
    static Step zero;
    free(meap->step);
    return zero;
}

void Meap_Free(Meap* const meap)
{
    free(meap->step);
}
