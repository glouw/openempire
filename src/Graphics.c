#include "Graphics.h"

const char* Graphics_GetString(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable) case name: return #name;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

uint8_t Graphics_GetHeight(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable) case name: return prio;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_IsWalkable(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable) case name: return walkable;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}
