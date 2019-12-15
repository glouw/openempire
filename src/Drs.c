#include "Drs.h"

#include "Util.h"

#include <string.h>
#include <assert.h>

void Drs_Print(const Drs drs)
{
    fprintf(stderr, "path        : %s\n", drs.path);
    fprintf(stderr, "copyright   : %s\n", drs.copyright);
    fprintf(stderr, "version     : %s\n", drs.version);
    fprintf(stderr, "ftype       : %s\n", drs.ftype);
    fprintf(stderr, "table_count : %d\n", drs.table_count);
    fprintf(stderr, "file_offset : %d\n", drs.file_offset);
    for(int32_t i = 0; i < drs.table_count; i++)
    {
        fprintf(stderr, "\tTable %d\n", i);
        Table_Print(drs.table[i]);
    }
}

Drs Drs_Load(const char* const path)
{
    static Drs zero;
    Drs drs = zero;
    drs.fp = fopen(path, "rb");
    drs.path = path;
    UTIL_FREAD(drs.copyright, sizeof(drs.copyright) - 1, drs.fp);
    UTIL_FREAD(drs.version, sizeof(drs.version) - 1, drs.fp);
    UTIL_FREAD(drs.ftype, sizeof(drs.ftype) - 1, drs.fp);
    UTIL_FREAD(&drs.table_count, 1, drs.fp);
    UTIL_FREAD(&drs.file_offset, 1, drs.fp);
    drs.table = UTIL_ALLOC(Table, drs.table_count);
    for(int32_t i = 0; i < drs.table_count; i++)
        drs.table[i] = Table_Load(drs.fp);
    for(int32_t i = 0; i < drs.table_count; i++)
    for(int32_t j = 0; j < drs.table[i].num_files; j++)
        drs.table[i].file[j] = File_Load(drs.fp);
#if 0
    Drs_Print(drs);
#endif
    return drs;
}

void Drs_Free(const Drs drs)
{
    for(int32_t i = 0; i < drs.table_count; i++)
        free(drs.table[i].file);
    free(drs.table);
    fclose(drs.fp);
}

static Table GetTable(const Drs drs, const int32_t table_index)
{
    assert(table_index <= drs.table_count);
    return drs.table[table_index];
}

Table Drs_GetBinaryTable(const Drs drs, const int32_t table_index)
{
    const Table table = GetTable(drs, table_index);
    assert(strcmp(table.file_extension, "anib") == 0);
    return table;
}

Table Drs_GetSlpTable(const Drs drs, const int32_t table_index)
{
    const Table table = GetTable(drs, table_index);
    assert(strcmp(table.file_extension, " pls") == 0);
    return table;
}
