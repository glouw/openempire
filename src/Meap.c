// The MIT License (MIT)
//
// Copyright (c) 2014 Robin Thomas
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    int32_t index = (meap->size)++;
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
        meap->step[0] = meap->step[--(meap->size)];
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
