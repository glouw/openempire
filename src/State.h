#pragma once

#include <stdbool.h>

typedef enum
{
    STATE_ATTACK,
    STATE_FALL,
    STATE_IDLE,
    STATE_DECAY,
    STATE_MOVE
}
State;

bool State_IsDead(const State);
