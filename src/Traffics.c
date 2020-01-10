#include "Traffics.h"

#include "Util.h"

Traffics Traffics_Init(const int max)
{
    static Traffics zero;
    Traffics traffics = zero;
    traffics.traffic = UTIL_ALLOC(Traffic, max);
    traffics.max = max;
    return traffics;
}

Traffics Traffics_Queue(Traffics traffics, Traffic traffic)
{
    const int32_t index = UTIL_WRAP(traffics.b, traffics.max);
    traffics.traffic[index] = traffic;
    traffics.b++;
    if(UTIL_WRAP(traffics.b, traffics.max)
    == UTIL_WRAP(traffics.a, traffics.max))
        traffics.a++;
    return traffics;
}

void Traffics_Free(const Traffics traffics)
{
    free(traffics.traffic);
}

int32_t Traffics_Size(const Traffics traffics)
{
    return traffics.b - traffics.a;
}

bool Traffics_Active(const Traffics traffics)
{
    return Traffics_Size(traffics) > 0;
}
