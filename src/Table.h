#pragma once

#include "File.h"

typedef struct
{
    char file_extension[5];
    int32_t file_info_offset;
    int32_t num_files;
    File* file;
}
Table;

Table Table_Load(FILE* const fp);

void Table_Print(const Table);

File Table_GetFile(const Table, const int32_t file_index);
