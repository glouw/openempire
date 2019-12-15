#include "Table.h"

#include <assert.h>

#include "Util.h"

Table Table_Load(FILE* const fp)
{
    static Table zero;
    Table table = zero;
    UTIL_FREAD(table.file_extension, sizeof(table.file_extension) - 1, fp);
    UTIL_FREAD(&table.file_info_offset, 1, fp);
    UTIL_FREAD(&table.num_files, 1, fp);
    table.file = UTIL_ALLOC(File, table.num_files);
    return table;
}

void Table_Print(const Table table)
{
    fprintf(stderr, "\t\tfile_extension   : %s\n", table.file_extension);
    fprintf(stderr, "\t\tfile_info_offset : %d\n", table.file_info_offset);
    fprintf(stderr, "\t\tnum_files        : %d\n", table.num_files);
    for(int32_t i = 0; i < table.num_files; i++)
    {
        fprintf(stderr, "\t\tFile %d\n", i);
        File_Print(table.file[i]);
    }
}

File Table_GetFile(const Table table, const int32_t file_index)
{
    assert(file_index <= table.num_files);
    return table.file[file_index];
}
