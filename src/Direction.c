#include "Direction.h"

// UNIT SPRITES ARE ONLY PACKED WITH 5 DIRECTIONS IN THE SLP FILES.
// THE REMAINING 3 DIRECTIONS ARE IMPROVISED BY FLIPPING 3 OF THE 5 DIRECTIONS.
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
        DIRECTION_SE, // NOTICE HOW SOUTH EAST IS NOW FIRST INSTEAD OF LAST - THIS IS ESSENTIALLY A 45 DEGREE ROTATION, EG. CART TO ISO.
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
