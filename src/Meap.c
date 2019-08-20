#include "Meap.h"

#include "Util.h"

#define LCHILD(x) ((2 * (x)) + 1)

#define RCHILD(x) ((2 * (x)) + 2)

#define PARENT(x) (((x) - 1) / 2)

static void Swap(Step* const n1, Step* const n2)
{
    const Step temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

static void Heapify(Meap* const meap, const int32_t index)
{
    int32_t smallest = (LCHILD(index) < meap->size && meap->step[LCHILD(index)].data < meap->step[index].data) ? LCHILD(index) : index;
    if(RCHILD(index) < meap->size && meap->step[RCHILD(index)].data < meap->step[smallest].data)
        smallest = RCHILD(index);
    if(smallest != index)
    {
        Swap(&(meap->step[index]), &(meap->step[smallest]));
        Heapify(meap, smallest);
    }
}

Meap Meap_Init(void)
{
    static Meap zero;
    return zero;
}

void Meap_Insert(Meap* const meap, const int32_t data, const Point point)
{
    if(meap->size)
        meap->step = UTIL_REALLOC(meap->step, Step, (meap->size + 1));
    else
        meap->step = UTIL_ALLOC(Step, 1);
    const Step node = { data, point };
    int32_t index = meap->size++;
    while(index && node.data < meap->step[PARENT(index)].data)
    {
        meap->step[index] = meap->step[PARENT(index)];
        index = PARENT(index);
    }
    meap->step[index] = node;
}

Step Meap_Delete(Meap* const meap)
{
    if(meap->size)
    {
        const Step node = meap->step[0];
        meap->step[0] = meap->step[--meap->size];
        meap->step = UTIL_REALLOC(meap->step, Step, meap->size);
        Heapify(meap, 0);
        return node;
    }
    else
    {
        static Step zero;
        free(meap->step);
        return zero;
    }
}

void Meap_Free(Meap* const meap)
{
    free(meap->step);
}
