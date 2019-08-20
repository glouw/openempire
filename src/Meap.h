// This Min-Heap (Meap) implementation is brought to you by Robin Thomas
// through the MIT license. I am not one for data structure theory.
//
// Meap serves as the foundation for the Field pathfinder.
//
// Heres the full license.
//
// --------------------------------------------------------------------------------
//
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

#pragma once

#include "Point.h"

#include <stdint.h>

typedef struct
{
    int32_t data;
    Point point;
}
Step;

typedef struct
{
    int32_t size;
    int32_t max;
    Step* step;
}
Meap;

Meap Meap_Init(void);

void Meap_Insert(Meap* const, const int32_t data, const Point);

Step Meap_Delete(Meap* const);

void Meap_Free(Meap*);
