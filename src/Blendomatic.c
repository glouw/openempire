#include "Blendomatic.h"

#include "Terrain.h"
#include "Util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Blendomatic Blendomatic_Load(const char* const path, const Registrar terrain)
{
    const Image image = terrain.animation[COLOR_GRY][FILE_DIRT].image[0];
    const Frame frame = terrain.animation[COLOR_GRY][FILE_DIRT].frame[0];
    char* const dat_path = Util_StringJoin(path, "blendomatic.dat");
    FILE* const fp = fopen(dat_path, "r");
    static Blendomatic zero;
    Blendomatic blendomatic = zero;
    UTIL_FREAD(&blendomatic.nr_blending_modes, 1, fp);
    UTIL_FREAD(&blendomatic.nr_tiles, 1, fp);
    blendomatic.mode = UTIL_ALLOC(Mode, blendomatic.nr_blending_modes);
    UTIL_CHECK(blendomatic.mode);
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
        blendomatic.mode[i] = Mode_Load(fp, blendomatic.nr_tiles, frame, image.outline_table);
    fclose(fp);
    free(dat_path);
    return blendomatic;
}

void Blendomatic_Free(const Blendomatic blendomatic)
{
    for(int32_t i = 0; i < (int32_t) blendomatic.nr_blending_modes; i++)
        Mode_Free(blendomatic.mode[i], blendomatic.nr_tiles);
    free(blendomatic.mode);
}
