#include "Registrar.h"

#include "File.h"
#include "Graphics.h"
#include "Interfac.h"
#include "Terrain.h"

#include "Util.h"

static Registrar New(const int32_t count, const int32_t files[], const int32_t file_count)
{
    static Registrar zero;
    Registrar registrar = zero;
    for(int32_t i = 0; i < (int32_t) COLOR_COUNT; i++)
        registrar.animation[i] = UTIL_ALLOC(Animation, count);
    registrar.files = files;
    registrar.file_count = file_count;
    return registrar;
}

typedef struct
{
    Registrar registrar;
    Slp slp;
    Palette palette;
    int32_t file;
    Color color;
    int32_t index;
}
ColorNeedle;

static int32_t LoadColorNeedle(void* const data)
{
    ColorNeedle* const needle = (ColorNeedle*) data;
    needle->registrar.animation[needle->index][needle->file] = Animation_Get(needle->slp, needle->palette, needle->color);
    return 0;
}

static void LoadColors(const Registrar registrar, const Slp slp, const Palette palette, const int32_t file)
{
    ColorNeedle needles[COLOR_COUNT];
    for(int32_t i = 0; i < (int32_t) COLOR_COUNT; i++)
    {
        needles[i].registrar = registrar;
        needles[i].slp = slp;
        needles[i].palette = palette;
        needles[i].file = file;
        needles[i].color = (Color) i;
        needles[i].index = i;
    }
    SDL_Thread* threads[COLOR_COUNT];
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        threads[i] = SDL_CreateThread(LoadColorNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        SDL_WaitThread(threads[i], NULL);
}

static Registrar LoadColoredAnimations(const Drs drs, const int32_t table_index, const Palette palette, const int32_t files[], const int32_t file_count)
{
    const Table table = Drs_GetSlpTable(drs, table_index);
    const Registrar registrar = New(table.num_files, files, file_count);
    for(int32_t i = 0; i < file_count; i++)
    {
        const int32_t file = files[i];
        if(file != FILE_NONE)
        {
            const Slp slp = Slp_Load(drs, table_index, file);
            LoadColors(registrar, slp, palette, file);
            Slp_Free(slp);
        }
    }
    return registrar;
}

static void FreeColors(const Registrar registrar, const int32_t file)
{
    for(int32_t j = 0; j < (int32_t) COLOR_COUNT; j++)
        Animation_Free(registrar.animation[j][file]);
}

void Registrar_Free(const Registrar registrar)
{
    for(int32_t i = 0; i < registrar.file_count; i++)
    {
        const int32_t file = registrar.files[i];
        if(file != FILE_NONE)
            FreeColors(registrar, file);
    }
    for(int32_t i = 0; i < (int32_t) COLOR_COUNT; i++)
        free(registrar.animation[i]);
}

Registrar Registrar_Load(const char* const path, const char* const drs_file_name, const int32_t table_index, const int32_t files[], const int32_t file_count)
{
    char* const drs_path = Util_StringJoin(path, drs_file_name);
    char* const interfac_path = Util_StringJoin(path, "interfac.drs");
    const Drs interfac = Drs_Load(interfac_path);
    const Drs drs = Drs_Load(drs_path);
    const Palette palette = Palette_Load(interfac, 0);
    const Registrar registrar = LoadColoredAnimations(drs, table_index, palette, files, file_count);
    Palette_Free(palette);
    Drs_Free(interfac);
    Drs_Free(drs);
    free(drs_path);
    free(interfac_path);
    return registrar;
}

Registrar Registrar_LoadInterfac(const char* const path)
{
    static const Interfac interfac[] = {
#define FILE_X(name, file, upgrade, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail, midding) name,
        FILE_X_INTERFAC
#undef FILE_X
    };
    return Registrar_Load(path, "interfac.drs", 1, (int32_t*) interfac, UTIL_LEN(interfac));
}

Registrar Registrar_LoadTerrain(const char* const path)
{
    static const Terrain terrain[] = {
#define FILE_X(name, file, upgrade, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail, midding) name,
        FILE_X_TERRAIN
#undef FILE_X
    };
    return Registrar_Load(path, "terrain.drs", 0, (int32_t*) terrain, UTIL_LEN(terrain));
}

Registrar Registrar_LoadGraphics(const char* const path)
{
    static const Graphics graphics[] = {
#define FILE_X(name, file, upgrade, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail, midding) name,
        FILE_X_GRAPHICS
#undef FILE_X
    };
    return Registrar_Load(path, "graphics.drs", 0, (int32_t*) graphics, UTIL_LEN(graphics));
}
