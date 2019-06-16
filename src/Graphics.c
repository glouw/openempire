#include "Graphics.h"

const char* Graphics_GetString(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return #name;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

uint8_t Graphics_GetHeight(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return prio;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_IsWalkable(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return walkable;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

Type Graphics_GetType(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return type;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return TYPE_NONE;
}

int32_t Graphics_GetMaxSpeed(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return max_speed;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetAcceleration(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return accel;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetHealth(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return health;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetAttack(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack) case name: return attack;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}
