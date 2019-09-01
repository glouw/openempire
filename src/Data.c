#include "Data.h"

#include "Util.h"

Data Data_Load(const char* const path)
{
    const int32_t t0 = SDL_GetTicks();
    static Data zero;
    Data data = zero;
    data.terrain = Registrar_LoadTerrain(path);
    data.graphics = Registrar_LoadGraphics(path);
    data.interfac = Registrar_LoadInterfac(path);
    data.blendomatic = Blendomatic_Load(path, data.terrain);
    const int32_t t1 = SDL_GetTicks();
    fprintf(stderr, "Data load time: %d ms\n", t1 - t0);
    return data;
}

void Data_Free(const Data data)
{
    Registrar_Free(data.terrain);
    Registrar_Free(data.graphics);
    Registrar_Free(data.interfac);
    Blendomatic_Free(data.blendomatic);
}
