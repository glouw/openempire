#include "Backup.h"

#include "Util.h"

static Backup Xor(Backup backup)
{
    for(int32_t i = 0; i < BACKUP_MAX; i++)
        backup.parities.parity[i] = Units_Xor(backup.units[i]);
    return backup;
}

static Backup Shift(Backup backup)
{
    const int32_t end = BACKUP_MAX - 1;
    for(int32_t i = end; i > 0; i--)
    {
        if(i == end)
            Units_Free(backup.units[end]);
        backup.units[i] = backup.units[i - 1];
    }
    return Xor(backup);
}

Backup Backup_Push(Backup backup, const Units units)
{
    backup = Shift(backup);
    backup.units[0] = Units_Copy(units);
    return backup;
}

Backup Backup_Init(void)
{
    static Backup zero;
    return zero;
}

void Backup_Free(const Backup backup)
{
    for(int32_t i = 0; i < BACKUP_MAX; i++)
        Units_Free(backup.units[i]);
}
