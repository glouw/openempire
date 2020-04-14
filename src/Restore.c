#include "Restore.h"

#include "Util.h"
#include "Config.h"

#include <assert.h>

#define HEADER_SIZE (sizeof(int32_t) + sizeof(Restore)) // REAL SIZE (size_real) OF WHOLE STREAM IS INCLUDED.

static Restore RecvPacked(const TCPsocket socket)
{
    const int32_t size_units = CONFIG_UNITS_MAX * sizeof(Unit);
    const int32_t size_max = HEADER_SIZE + size_units;
    uint8_t* const buffer = UTIL_ALLOC(uint8_t, size_max);
    const int32_t* const size_real = (int32_t*) &buffer[0];
    const Restore* const restore   = (Restore*) &buffer[4];
    const Unit*    const unit      = (Unit*)    &buffer[4 + sizeof(Restore)];
    int32_t bytes = 0;
    while(bytes < size_max)
    {
        bytes += SDLNet_TCP_Recv(socket, &buffer[bytes], size_max);
        if(bytes >= *size_real) // EARLY EXIT WHEN ALL STREAM BYTES ARE RELIABLY RECVD OVER TCP.
            break;
    }
    assert(bytes == *size_real);
    Restore out = *restore;
    if(out.count > 0)
    {
        out.unit = UTIL_ALLOC(Unit, out.count);
        for(int32_t i = 0; i < out.count; i++)
            out.unit[i] = unit[i];
    }
    free(buffer);
    return out;
}

Restore Restore_Recv(const TCPsocket socket)
{
    static Restore zero;
    return socket
        ? RecvPacked(socket)
        : zero;
}

void Restore_Send(const Restore restore, const TCPsocket socket)
{
    const int32_t size_units = restore.count * sizeof(*restore.unit);
    const int32_t size_real = HEADER_SIZE + size_units;
    UTIL_TCP_SEND(socket, &size_real);
    UTIL_TCP_SEND(socket, &restore);
    SDLNet_TCP_Send(socket, restore.unit, size_units);
}

void Restore_Free(const Restore restore)
{
    free(restore.unit);
}

void Restore_Print(const Restore restore)
{
    printf("%d\n", restore.count);
    printf("%d\n", restore.cycles);
    printf("%d\n", restore.id_next);
    printf("%d\n", restore.command_group_next);
}
