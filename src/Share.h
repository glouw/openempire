#pragma once

#include "Color.h"
#include "Status.h"
#include "Motive.h"
#include "Bits.h"

typedef struct
{
    Status status;
    Motive motive;
    Bits bits;
    Bits busy;
    int32_t select_count;
    int32_t select_count_inanimate;
    int32_t command_group_next;
}
Share;
