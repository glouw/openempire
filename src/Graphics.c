#include "Graphics.h"

const char* Graphics_GetString(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return #name;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

uint8_t Graphics_GetHeight(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return prio;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetWalkable(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return walkable;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

Type Graphics_GetType(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return type;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return TYPE_NONE;
}

int32_t Graphics_GetMaxSpeed(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return max_speed;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetAcceleration(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return accel;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetHealth(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return health;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetAttack(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return attack;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetWidth(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return width;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetRotatable(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return rotatable;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetSingleFrame(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return single_frame;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetMultiState(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return multi_state;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetExpire(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: return expire;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

Point Graphics_GetDimensions(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state, expire, dimensions) case name: { const Point dim = dimensions; return dim; }
        FILE_X_GRAPHICS
#undef FILE_X
    }
    Point zero = { 0,0 };
    return zero;
}
