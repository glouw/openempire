#include "Restore.h"

#include "Util.h"
#include "Config.h"

#include <assert.h>

#define RESTORE_COMPLETE (0xAB)

static int32_t GetRestoreHeaderByteSize(const Restore restore)
{
    return sizeof(int32_t) // REAL SIZE (size_real) OF WHOLE STREAM IS INCLUDED.
         + sizeof(restore.count)
         + sizeof(restore.cycles)
         + sizeof(restore.id_next)
         + sizeof(restore.stamp);
}

static Restore RecvPacked(const TCPsocket socket)
{
    static Restore zero;
    Restore restore = zero;
    const int32_t size_units = CONFIG_UNITS_MAX * sizeof(*restore.unit);
    const int32_t size_header = GetRestoreHeaderByteSize(restore);
    const int32_t size_max = size_header + size_units;
    uint8_t* const buffer = UTIL_ALLOC(uint8_t, size_max);
    /* ---------- HEADER --------- */
    const int32_t* const size_real = (int32_t*) &buffer[ 0]; // THE REAL SIZE OF THE INCOMING STREAM
    const int32_t* const count     = (int32_t*) &buffer[ 4]; // WAS CHOSEN TO BE INCLUDED IN THE STREAM.
    const int32_t* const cycles    = (int32_t*) &buffer[ 8]; // IT IS CHICKEN AND EGG, BUT IT IS MORE RELIABLE.
    const int32_t* const id_next   = (int32_t*) &buffer[12]; // THAN SPLITTING THE RECIEVER INTO TWO RECV CALLS.
    const Share  * const stamp     = (Share  *) &buffer[16];
    /* ---------- UNITS ---------- */
    const Unit * const unit = (Unit*) &buffer[16 + sizeof(restore.stamp)];
    int32_t bytes = 0;
    while(bytes < size_max)
    {
        bytes += SDLNet_TCP_Recv(socket, &buffer[bytes], size_max);
        if(bytes >= *size_real) // EARLY EXIT WHEN ALL STREAM BYTES ARE RELIABLY RECVD OVER TCP.
            break;
    }
    assert(bytes == *size_real);
    restore.count = *count;
    if(restore.count > 0)
    {
        restore.unit = UTIL_ALLOC(Unit, restore.count);
        for(int32_t i = 0; i < restore.count; i++)
            restore.unit[i] = unit[i];
    }
    restore.id_next = *id_next;
    restore.cycles = *cycles;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        restore.stamp[i] = stamp[i];
    free(buffer);
    const uint8_t ack = RESTORE_COMPLETE;
    assert(UTIL_TCP_SEND(socket, &ack) == sizeof(uint8_t));
    SDL_Delay(250); // XXX. THIS DELAY MAY NEED TO ACCOUNT FOR PING TIMES.
    return restore;
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
    const int32_t size_header = GetRestoreHeaderByteSize(restore);
    const int32_t size_units = restore.count * sizeof(*restore.unit);
    const int32_t size_real = size_header + size_units;
    /* ---------- HEADER --------- */
    assert(UTIL_TCP_SEND(socket, &size_real)       == sizeof(int32_t));
    assert(UTIL_TCP_SEND(socket, &restore.count)   == sizeof(int32_t));
    assert(UTIL_TCP_SEND(socket, &restore.cycles)  == sizeof(int32_t));
    assert(UTIL_TCP_SEND(socket, &restore.id_next) == sizeof(int32_t));
    assert(UTIL_TCP_SEND(socket, &restore.stamp)   == sizeof(restore.stamp));
    /* ---------- UNITS ---------- */
    SDLNet_TCP_Send(socket, restore.unit, size_units);
    /* ----- ASSERT COMPLETE ----- */
    uint8_t ack = 0;
    assert(UTIL_TCP_RECV(socket, &ack) == sizeof(uint8_t));
    assert(ack == RESTORE_COMPLETE);
}

void Restore_Free(const Restore restore)
{
    free(restore.unit);
}
