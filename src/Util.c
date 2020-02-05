#include "Util.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <execinfo.h>

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
    strcat(joined, a);
    strcat(joined, b);
    return joined;
}

char* Util_StringDup(const char* s)
{
    const int32_t len = strlen(s) + 1;
    char* const d = UTIL_ALLOC(char, len);
    strcpy(d, s);
    return d;
}

bool Util_StringEqual(const char* const a, const char* const b)
{
    return strcmp(a, b) == 0;
}

int32_t Util_Sqrt(const int64_t val)
{
    uint32_t a = 0;
    uint32_t b = 0;
    if(val < 2)
        return (int32_t) val;
    a = 1255;
    for(int32_t i = 0; i < 8; i++)
    {
        b = val / a;
        a = (a + b) / 2;
    }
    return a;
}

int32_t Util_Pow(const int32_t value, const int32_t n)
{
    int32_t out = value;
    for(int32_t i = 0; i < n - 1; i++)
        out *= value;
    return out;
}

static uint32_t next;

void Util_Srand(const int32_t seed)
{
    next = seed;
}

uint16_t Util_Rand(void)
{
    next = next * 1103515245 + 12345;
    return (uint16_t) (next / 65536) % 32768;
}

bool Util_FlipCoin(void)
{
    return Util_Rand() % 2;
}

void Util_Trace(void)
{
    void *array[10];
    const size_t size = backtrace(array, 10);
    char** const strings = backtrace_symbols(array, size);
    printf("Obtained %zd stack frames.\n", size);
    for(size_t i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    free(strings);
}
