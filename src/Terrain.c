#include "Terrain.h"

const char* Terrain_GetString(const Terrain terrain)
{
    switch(terrain)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state) case name: return #name;
        FILE_X_TERRAIN
#undef FILE_X
    }
    return 0;
}

uint8_t Terrain_GetHeight(const Terrain terrain)
{
    switch(terrain)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state) case name: return prio;
        FILE_X_TERRAIN
#undef FILE_X
    }
    return 0;
}

bool Terrain_IsWalkable(const Terrain terrain)
{
    switch(terrain)
    {
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health, attack, width, rotatable, single_frame, multi_state) case name: return walkable;
        FILE_X_TERRAIN
#undef FILE_X
    }
    return 0;
}
