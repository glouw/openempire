#include "Direction.h"

// Unit sprites are only packed with 5 directions in the SLP files.
// The remaining 3 directions are improvised by flipping 3 of the 5 directions.
Direction Direction_Fix(const Direction dir, bool* const flip_vert)
{
    if(dir > DIRECTION_N)
    {
        if(flip_vert != NULL)
            *flip_vert = true;
        switch(dir)
        {
        case DIRECTION_NE: return DIRECTION_NW;
        case DIRECTION_E : return DIRECTION_W;
        case DIRECTION_SE: return DIRECTION_SW;
        default:
            break;
        }
    }
    return dir;
}

Direction Direction_GetCart(const Point point)
{
    const Point flip = { point.y, point.x };
    const int32_t slope_y = abs(Point_Slope(point));
    const int32_t slope_x = abs(Point_Slope(flip));
    if(point.x >= 0 && point.y >= 0) return (slope_x >= 2) ? DIRECTION_E : (slope_y >= 2) ? DIRECTION_S : DIRECTION_SE;
    if(point.x  < 0 && point.y >= 0) return (slope_x >= 2) ? DIRECTION_W : (slope_y >= 2) ? DIRECTION_S : DIRECTION_SW;
    if(point.x  < 0 && point.y  < 0) return (slope_x >= 2) ? DIRECTION_W : (slope_y >= 2) ? DIRECTION_N : DIRECTION_NW;
    return (slope_x >= 2) ? DIRECTION_E : (slope_y >= 2) ? DIRECTION_N : DIRECTION_NE;
}

Direction Direction_CartToIso(const Direction dir)
{
    const int32_t index = (int32_t) dir;
    const Direction dirs[] = {
        DIRECTION_SE, // Notice how south east is now first instead of last - this is essentially a 45 degree rotation, eg. cart to iso.
        DIRECTION_S,
        DIRECTION_SW,
        DIRECTION_W,
        DIRECTION_NW,
        DIRECTION_N,
        DIRECTION_NE,
        DIRECTION_E,
    };
    return dirs[index];
}
