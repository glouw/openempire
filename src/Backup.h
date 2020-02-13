#pragma once

#include "Units.h"
#include "Parity.h"

#define BACKUP_MAX (16)

typedef struct
{
    Units units[BACKUP_MAX];
    Parity parity[BACKUP_MAX];
}
Backup;

Backup Backup_Push(Backup, const Units, const int32_t cycles);

Backup Backup_Init(void);

void Backup_Free(const Backup);
