#pragma once

#include <stdbool.h>

typedef struct
{
    uint32_t
        mouse_l        : 1,
        mouse_r        : 1,
        mouse_ld       : 1,
        mouse_lu       : 1,
        mouse_rd       : 1,
        mouse_ru       : 1,
        key_q          : 1,
        key_w          : 1,
        key_e          : 1,
        key_r          : 1,
        key_t          : 1,
        key_a          : 1,
        key_s          : 1,
        key_d          : 1,
        key_f          : 1,
        key_g          : 1,
        key_z          : 1,
        key_x          : 1,
        key_c          : 1,
        key_v          : 1,
        key_b          : 1,
        key_1          : 1,
        key_2          : 1,
        key_3          : 1,
        key_left_ctrl  : 1,
        key_left_shift : 1,
        key_left_alt   : 1,
        tab            : 1;
}
Event;
