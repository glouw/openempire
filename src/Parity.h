#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint64_t xorred;
    int32_t cycles;
}
Parity;

bool Parity_IsEqual(const Parity a, const Parity b);

bool Parity_IsZero(const Parity);
