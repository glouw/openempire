#include "Restore.h"

#include "Util.h"

Restore Restore_Recv(TCPsocket socket)
{
    static Restore zero;
    Restore restore = zero;
    if(socket)
    {
        int32_t header_size = 0;
        UTIL_TCP_RECV(socket, &header_size);
        uint8_t* const buffer = UTIL_ALLOC(uint8_t, header_size);
        for(int32_t bytes = 0; bytes < header_size;)
            bytes += SDLNet_TCP_Recv(socket, &buffer[bytes], header_size);
        const int32_t* const count  = (int32_t*) &buffer[0];
        const int32_t* const cycles = (int32_t*) &buffer[4];
        const Share  * const stamp  = (Share  *) &buffer[8];
        const Unit   * const unit   = (Unit   *) &buffer[8 + sizeof(restore.stamp)];
        restore.count = *count;
        restore.cycles = *cycles;
        restore.unit = UTIL_ALLOC(Unit, restore.count);
        for(int32_t i = 0; i < COLOR_COUNT; i++)
            restore.stamp[i] = stamp[i];
        for(int32_t i = 0; i < restore.count; i++)
            restore.unit[i] = unit[i];
        free(buffer);
    }
    return restore;
}

void Restore_Send(const Restore restore, TCPsocket socket)
{
    if(socket)
    {
        const int32_t size = restore.count * sizeof(*restore.unit);
        const int32_t header_size =
            sizeof(restore.count)
          + sizeof(restore.cycles)
          + size
          + sizeof(restore.stamp);
        UTIL_TCP_SEND(socket, &header_size);
        UTIL_TCP_SEND(socket, &restore.count);
        UTIL_TCP_SEND(socket, &restore.cycles);
        UTIL_TCP_SEND(socket, &restore.stamp);
        SDLNet_TCP_Send(socket, restore.unit, size);
    }
}

void Restore_Free(const Restore restore)
{
    free(restore.unit);
}
