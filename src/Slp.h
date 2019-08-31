#pragma once

#include "Frame.h"
#include "Image.h"
#include "Drs.h"

#include <stdint.h>

typedef struct
{
    Frame* frame;
    Image* image;
    char version[5];
    char comment[25];
    uint32_t num_frames;
}
Slp;

void Slp_Print(const Slp);

Slp Slp_Load(const Drs, const int32_t table_index, const int32_t file_index);

void Slp_Free(const Slp);
