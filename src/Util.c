#include "Util.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>

void Util_Bomb(const char* const message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(stdout, message, args);
    va_end(args);
    exit(1);
}

char* Util_StringJoin(const char* const a, const char* const b)
{
    const int32_t len = strlen(a) + strlen(b) + 1;
    char* const joined = UTIL_ALLOC(char, len);
    UTIL_CHECK(joined);
    strcat(joined, a);
    strcat(joined, b);
    return joined;
}

char* Util_StringDup(const char* s)
{
    const int32_t len = strlen(s) + 1;
    char* const d = UTIL_ALLOC(char, len);
    UTIL_CHECK(d);
    strcpy(d, s);
    return d;
}

bool Util_StringEqual(const char* const a, const char* const b)
{
    return strcmp(a, b) == 0;
}

int64_t Util_Sqrt(const int64_t val)
{
    uint64_t a = 0;
    uint64_t b = 0;
    if(val < 2)
        return val;
    a = 1255;
    for(int32_t i = 0; i < 8; i++)
    {
        b = val / a;
        a = (a + b) / 2;
    }
    return a;
}

uint16_t Util_Rand(void)
{
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return (uint16_t) (next / 65536) % 32768;
}

int32_t Util_Time(void)
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec * (int32_t) 1e6 + stamp.tv_usec;
}
