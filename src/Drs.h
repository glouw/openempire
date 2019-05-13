#pragma once

#include "Table.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    FILE* fp;
    const char* path;
    char copyright[41];
    char version[5];
    char ftype[13];
    int32_t table_count;
    int32_t file_offset;
    Table* table;
}
Drs;

void Drs_Print(const Drs);

Drs Drs_Load(const char* const path);

void Drs_Free(const Drs);

Table Drs_GetBinaryTable(const Drs, const int32_t table_index);

Table Drs_GetSlpTable(const Drs, const int32_t table_index);
