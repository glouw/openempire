#pragma once

#include "File.h"

typedef struct
{
    File* file;
    int32_t file_info_offset;
    int32_t num_files;
    char file_extension[5];
}
Table;

Table Table_Load(FILE* const fp);

void Table_Print(const Table);

File Table_GetFile(const Table, const int32_t file_index);
