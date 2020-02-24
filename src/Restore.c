#include "Restore.h"

#include "Util.h"

#include <assert.h>

#define READY (0xCAFEBABE)

static uint8_t* RecvBytes(const TCPsocket socket)
{
    int32_t size_total = 0;
    assert(UTIL_TCP_RECV(socket, &size_total) == 4);
    const uint32_t ready = READY;
    UTIL_TCP_SEND(socket, &ready);
    uint8_t* const buffer = UTIL_ALLOC(uint8_t, size_total);
    for(int32_t bytes = 0; bytes < size_total;)
        bytes += SDLNet_TCP_Recv(socket, &buffer[bytes], size_total);
    return buffer;
}

static Restore Pack(uint8_t* const buffer)
{
    static Restore zero;
    Restore restore = zero;
    const int32_t* const count  = (int32_t*) &buffer[0];
    const int32_t* const cycles = (int32_t*) &buffer[4];
    const Share  * const stamp  = (Share  *) &buffer[8];
    const Unit   * const unit   = (Unit   *) &buffer[8 + sizeof(restore.stamp)];
    restore.count = *count;
    if(restore.count > 0)
    {
        restore.unit = UTIL_ALLOC(Unit, restore.count);
        for(int32_t i = 0; i < restore.count; i++)
            restore.unit[i] = unit[i];
    }
    restore.cycles = *cycles;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        restore.stamp[i] = stamp[i];
    return restore;
}

Restore Restore_Recv(const TCPsocket socket)
{
    if(socket)
    {
        uint8_t* const buffer = RecvBytes(socket);
        const Restore restore = Pack(buffer);
        free(buffer);
        return restore;
    }
    static Restore zero;
    return zero;
}

void Restore_Send(const Restore restore, const TCPsocket socket)
{
    if(socket)
    {
        const int32_t size = restore.count * sizeof(*restore.unit);
        const int32_t size_total =
            sizeof(restore.count)
          + sizeof(restore.cycles)
          + size
          + sizeof(restore.stamp);
        UTIL_TCP_SEND(socket, &size_total);
        uint32_t ready = 0;
        UTIL_TCP_RECV(socket, &ready);
        assert(ready == READY);
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
