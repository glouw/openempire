#include "Palette.h"

#include "Util.h"

void Palette_Print(const Palette palette)
{
    fprintf(stderr, "label   : %s\n", palette.label);
    fprintf(stderr, "version : %s\n", palette.version);
    for(int32_t i = 0; i < palette.count; i++)
    {
        const uint32_t color = palette.color[i];
        fprintf(stderr, "%3d : %3d %3d %3d\n", i, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    fprintf(stderr, "count   : %d\n", palette.count);
}

Palette Palette_Load(const Drs interfac, const int32_t palette_offset)
{
    const Table table = Drs_GetBinaryTable(interfac, 0);
    const File file = Table_GetFile(table, 67 + palette_offset);
    fseek(interfac.fp, file.data_offset, SEEK_SET);
    static Palette zero;
    Palette palette = zero;
    fscanf(interfac.fp, "%15s\n", palette.label);
    fscanf(interfac.fp, "%7s\n", palette.version);
    fscanf(interfac.fp, "%d\n", &palette.count);
    palette.color = UTIL_ALLOC(uint32_t, palette.count);
    UTIL_CHECK(palette.color);
    for(int32_t i = 0; i < palette.count; i++)
    {
        int32_t r = 0;
        int32_t g = 0;
        int32_t b = 0;
        fscanf(interfac.fp, "%d %d %d\n", &r, &g, &b);
        palette.color[i] = (r << 16) | (g << 8) | b;
    }
    return palette;
}

void Palette_Free(const Palette palette)
{
    free(palette.color);
}
