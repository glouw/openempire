#include "Event.h"

int32_t Event_GetIndex(const Event event)
{
    if(event.key_left_alt)
    {
        if(event.key_q) return  0;
        if(event.key_w) return  1;
        if(event.key_e) return  2;
        if(event.key_r) return  3;
        if(event.key_t) return  4;
        if(event.key_a) return  5;
        if(event.key_s) return  6;
        if(event.key_d) return  7;
        if(event.key_f) return  8;
        if(event.key_g) return  9;
        if(event.key_z) return 10;
        if(event.key_x) return 11;
        if(event.key_c) return 12;
        if(event.key_v) return 13;
        if(event.key_b) return 14;
    }
    return -1;
}
