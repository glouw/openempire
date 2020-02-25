#include "Data.h"

#include "Util.h"

static void Check(const Data data)
{
    if(data.terrain.file_count == 0
    || data.graphics.file_count == 0
    || data.interfac.file_count == 0)
        Util_Bomb("One or more data files failed to successfuly load...\n");
}

Data Data_Load(const char* const path)
{
    static Data zero;
    Data data = zero;
    data.terrain = Registrar_LoadTerrain(path);
    data.graphics = Registrar_LoadGraphics(path);
    data.interfac = Registrar_LoadInterfac(path);
    data.blendomatic = Blendomatic_Load(path, data.terrain);
    Check(data);
    return data;
}

void Data_Free(const Data data)
{
    Registrar_Free(data.terrain);
    Registrar_Free(data.graphics);
    Registrar_Free(data.interfac);
    Blendomatic_Free(data.blendomatic);
}
