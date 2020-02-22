#include "Restore.h"

#include "Util.h"

static Points RecvPath(TCPsocket socket)
{
    Points path;
    UTIL_TCP_RECV(socket, &path.max);
    UTIL_TCP_RECV(socket, &path.count);
    path.point = UTIL_ALLOC(Point, path.max);
    for(int32_t i = 0; i < path.count; i++)
        UTIL_TCP_RECV(socket, &path.point[i]);
    return path;
}

static void SendPath(TCPsocket socket, const Points path)
{
    UTIL_TCP_SEND(socket, &path.max);
    UTIL_TCP_SEND(socket, &path.count);
    for(int32_t i = 0; i < path.count; i++)
        UTIL_TCP_SEND(socket, &path.point[i]);
}

Restore Restore_Recv(TCPsocket socket)
{
    static Restore zero;
    Restore restore = zero;
    if(socket)
    {
        UTIL_TCP_RECV(socket, &restore.count);
        UTIL_TCP_RECV(socket, &restore.cycles);
        restore.unit = UTIL_ALLOC(Unit, restore.count);
        for(int32_t i = 0; i < restore.count; i++) UTIL_TCP_RECV(socket, &restore.unit[i]);
        for(int32_t i = 0; i < restore.count; i++) restore.unit[i].path = RecvPath(socket);
    }
    return restore;
}

void Restore_Send(const Restore restore, TCPsocket socket)
{
    if(socket)
    {
        UTIL_TCP_SEND(socket, &restore.count);
        UTIL_TCP_SEND(socket, &restore.cycles);
        for(int32_t i = 0; i < restore.count; i++) UTIL_TCP_SEND(socket, &restore.unit[i]);
        for(int32_t i = 0; i < restore.count; i++) SendPath(socket, restore.unit[i].path);
    }
}

void Restore_Free(const Restore restore)
{
    free(restore.unit);
}
