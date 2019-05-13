#pragma once

#include "Frame.h"
#include "Image.h"
#include "Drs.h"

#include <stdint.h>

typedef struct
{
    char version[5];
    uint32_t num_frames;
    char comment[25];
    Frame* frame;
    Image* image;
}
Slp;

void Slp_Print(const Slp);

Slp Slp_Load(const Drs, const int32_t table_index, const int32_t file_index);

void Slp_Free(const Slp);
