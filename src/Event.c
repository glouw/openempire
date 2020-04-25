#include "Event.h"

int32_t Event_GetIndex(const Event event)
{
    int32_t index = -1;
    if(event.key_q) index =  0;
    if(event.key_w) index =  1;
    if(event.key_e) index =  2;
    if(event.key_r) index =  3;
    if(event.key_t) index =  4;
    if(event.key_a) index =  5;
    if(event.key_s) index =  6;
    if(event.key_d) index =  7;
    if(event.key_f) index =  8;
    if(event.key_g) index =  9;
    if(event.key_z) index = 10;
    if(event.key_x) index = 11;
    if(event.key_c) index = 12;
    if(event.key_v) index = 13;
    if(event.key_b) index = 14;
    return index;
}
