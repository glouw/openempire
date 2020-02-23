#pragma once

#include "Trigger.h"

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t Bits;

Bits Bits_Set(Bits, const Trigger);

bool Bits_Get(Bits, const Trigger);

void Bits_Print(const Bits);
