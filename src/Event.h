#pragma once

#include <stdbool.h>

typedef struct
{
    bool mouse_l        : 1;
    bool mouse_r        : 1;
    bool mouse_ld       : 1;
    bool mouse_lu       : 1;
    bool mouse_rd       : 1;
    bool mouse_ru       : 1;
    bool key_left_shift : 1;
    bool key_q          : 1;
    bool key_w          : 1;
    bool key_e          : 1;
    bool key_r          : 1;
    bool key_t          : 1;
    bool key_a          : 1;
    bool key_s          : 1;
    bool key_d          : 1;
    bool key_f          : 1;
    bool key_g          : 1;
    bool key_z          : 1;
    bool key_x          : 1;
    bool key_c          : 1;
    bool key_v          : 1;
    bool key_b          : 1;
    bool key_1          : 1;
    bool key_2          : 1;
    bool key_3          : 1;
    bool key_left_ctrl  : 1;
    bool tab            : 1;
}
Event;
