#include "Backup.h"

#include "Util.h"

static Backup Shift(Backup backup)
{
    const int32_t end = BACKUP_MAX - 1;
    for(int32_t i = end; i > 0; i--)
    {
        if(i == end)
            Units_Free(backup.units[end]);
        const int32_t next = i - 1;
        backup.units[i] = backup.units[next];
        backup.parity[i] = backup.parity[next];
    }
    return backup;
}

Backup Backup_Push(Backup backup, const Units units, const int32_t cycles)
{
    backup = Shift(backup);
    backup.parity[0].xorred = Units_Xor(units);
    backup.parity[0].cycles = cycles;
    backup.units[0] = Units_Copy(units);
    return backup;
}

Backup Backup_Init(void)
{
    static Backup zero;
    return zero;
}

Backup Backup_Free(Backup backup)
{
    static Units zero;
    static Parity none;
    for(int32_t i = 0; i < BACKUP_MAX; i++)
    {
        Units_Free(backup.units[i]);
        backup.units[i] = zero;
        backup.parity[i] = none;
    }
    return backup;
}
