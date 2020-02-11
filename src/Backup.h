#include "Units.h"

#include <stdint.h>

#define BACKUP_MAX (8)

typedef struct
{
    uint64_t parity[BACKUP_MAX];
}
Parities;

typedef struct
{
    Units units[BACKUP_MAX];
    Parities parities;
}
Backup;

Backup Backup_Push(Backup, const Units);

Backup Backup_Init(void);

void Backup_Free(const Backup);
