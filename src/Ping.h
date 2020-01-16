#pragma once

#include "Args.h"

#include <stdint.h>

int32_t Ping_Get(void);

void Ping_Init(const Args args);

void Ping_Shutdown(void);
