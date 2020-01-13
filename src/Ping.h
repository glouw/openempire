#pragma once

#include <stdint.h>

int32_t Ping_Get(void);

int32_t Ping_Ping(void* const data);

void Ping_Init(void);

void Ping_Shutdown(void);
