#include "State.h"

bool State_IsDead(const State state)
{
    return state == STATE_FALL || state == STATE_DECAY;
}
