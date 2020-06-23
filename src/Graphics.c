#include "Graphics.h"

#include "Interfac.h"
#include "Util.h"
#include "Config.h"

const char* Graphics_GetString(const Graphics graphics)
{
#if CONFIG_DISABLE_STRINGS == 1
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return #name;
        FILE_X_GRAPHICS
#undef FILE_X
    }
#else
    (void) graphics;
#endif
    return "N/A";
}

uint8_t Graphics_GetHeight(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return prio;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetWalkable(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return walkable;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

Type Graphics_GetType(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return type;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return TYPE_NONE;
}

int32_t Graphics_GetMaxSpeed(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return max_speed;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetHealth(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return health;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetAttack(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return attack;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

int32_t Graphics_GetWidth(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return width;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return 0;
}

bool Graphics_GetSingleFrame(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return single_frame;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

bool Graphics_GetMultiState(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return multi_state;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

bool Graphics_GetExpire(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return expire;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

Point Graphics_GetDimensions(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: { const Point dim = dimensions; return dim; }
        FILE_X_GRAPHICS
#undef FILE_X
    }
    static Point zero;
    return zero;
}

bool Graphics_GetInanimate(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return inanimate;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

Action Graphics_GetAction(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return action;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return ACTION_NONE;
}

bool Graphics_GetDetail(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return detail;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

Graphics Graphics_GetUpgrade(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return upgrade;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return FILE_GRAPHICS_NONE;
}

bool Graphics_GetResource(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return resource;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return false;
}

bool Graphics_EqualDimension(const Graphics file, Point dimensions)
{
    const int32_t min = UTIL_MIN(dimensions.x, dimensions.y);
    dimensions.x = min;
    dimensions.y = min;
    const Point _1x1_ = FILE_DIMENSIONS_1X1;
    const Point _2x2_ = FILE_DIMENSIONS_2X2;
    const Point _3x3_ = FILE_DIMENSIONS_3X3;
    const Point _4x4_ = FILE_DIMENSIONS_4X4;
    switch(file)
    {
        default:
        // 1X1, BEING THE SMALLEST, IS MOST SANE DEFAULT CASE.
        case FILE_GRAPHICS_RUBBLE_1X1: case FILE_GRAPHICS_CONSTRUCTION_1X1: return Point_Equal(_1x1_, dimensions);
        case FILE_GRAPHICS_RUBBLE_2X2: case FILE_GRAPHICS_CONSTRUCTION_2X2: return Point_Equal(_2x2_, dimensions);
        case FILE_GRAPHICS_RUBBLE_3X3: case FILE_GRAPHICS_CONSTRUCTION_3X3: return Point_Equal(_3x3_, dimensions);
        case FILE_GRAPHICS_RUBBLE_4X4: case FILE_GRAPHICS_CONSTRUCTION_4X4: return Point_Equal(_4x4_, dimensions);
    }
}

Type Graphics_GetCreator(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: return creator;
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return TYPE_NONE;
}

Graphics Graphics_GetGraphicsState(const Graphics graphics, const State index)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: { State s[] = states; return s[index]; }
        FILE_X_GRAPHICS
#undef FILE_X
    }
    return FILE_GRAPHICS_NONE;
}

Point Graphics_GetShift(const Graphics graphics)
{
    switch(graphics)
    {
#define FILE_X(name, file, shift, states, upgrade, creator, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail) case name: { const Point s = shift; return s; }
        FILE_X_GRAPHICS
#undef FILE_X
    }
    const Point none = FILE_SHIFT_0;
    return none;
}
