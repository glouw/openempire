#pragma once

#include "Video.h"

#include <stdint.h>

typedef struct
{
    Video video;
    char** message;
    int32_t a; // Circular buffer.
    int32_t b;
}
Log;

void Log_Init(const Video);

void Log_Append(const char* const format, ...);

void Log_Dump(void);
