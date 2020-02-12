#include "Parity.h"

bool Parity_IsEqual(const Parity a, const Parity b)
{
    return a.xorred == b.xorred
        && a.cycles == b.cycles;
}

bool Parity_IsZero(const Parity parity)
{
    static Parity zero;
    return Parity_IsEqual(parity, zero);
}
