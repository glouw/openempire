#include "Data.h"

#include "Util.h"

Data Data_Load(const char* const path)
{
    const int32_t t0 = SDL_GetTicks();
    static Data zero;
    Data data = zero;
    data.terrain = Registrar_LoadTerrain(path);
    if (data.terrain.file_count == 0) {
        fprintf(stderr, "Failed to load terrain\n");
        return data;
    }
    data.graphics = Registrar_LoadGraphics(path);
    if (data.graphics.file_count == 0) {
        fprintf(stderr, "Failed to load graphics\n");
        return data;
    }
    data.interfac = Registrar_LoadInterfac(path);
    if (data.interfac.file_count == 0) {
        fprintf(stderr, "Failed to load interfac\n");
        return data;
    }
    data.blendomatic = Blendomatic_Load(path, data.terrain);
    const int32_t t1 = SDL_GetTicks();
    fprintf(stderr, "Data load time: %d ms\n", t1 - t0);
    data.loaded = 1;
    return data;
}

void Data_Free(const Data data)
{
    Registrar_Free(data.terrain);
    Registrar_Free(data.graphics);
    Registrar_Free(data.interfac);
    Blendomatic_Free(data.blendomatic);
}
