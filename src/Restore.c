#include "Restore.h"

#include "Util.h"

// XXX. THINK OF A WAY TO MAKE A NICE MACRO FOR THESE SIZEOF OPERATORS.

static Points RecvPath(TCPsocket socket)
{
    Points path;
    SDLNet_TCP_Recv(socket, &path.max, sizeof(path.max));
    SDLNet_TCP_Recv(socket, &path.count, sizeof(path.count));
    path.point = UTIL_ALLOC(Point, path.max);
    SDLNet_TCP_Recv(socket, path.point, sizeof(*path.point) * path.count);
    return path;
}

static void SendPath(TCPsocket socket, const Points path)
{
    SDLNet_TCP_Send(socket, &path.max, sizeof(path.max));
    SDLNet_TCP_Send(socket, &path.count, sizeof(path.count));
    SDLNet_TCP_Send(socket, path.point, sizeof(*path.point) * path.count);
}

Restore Restore_Recv(TCPsocket socket)
{
    static Restore zero;
    if(socket)
    {
        int32_t count = 0;
        int32_t max = 0;
        SDLNet_TCP_Recv(socket, &count, sizeof(count));
        SDLNet_TCP_Recv(socket, &max, sizeof(max));
        Unit* const unit = UTIL_ALLOC(Unit, max);
        SDLNet_TCP_Recv(socket, unit, sizeof(*unit) * count);
        for(int32_t i = 0; i < count; i++)
        {
            printf("RECV PATH %d / %d\n", i, count);
            unit->path = RecvPath(socket);
        }
        const Restore restore = { unit, count, max };
        return restore;
    }
    return zero;
}

void Restore_Send(const Restore restore, TCPsocket socket)
{
    if(socket)
    {
        SDLNet_TCP_Send(socket, &restore.count, sizeof(restore.count));
        SDLNet_TCP_Send(socket, &restore.max, sizeof(restore.max));
        SDLNet_TCP_Send(socket, restore.unit, sizeof(*restore.unit) * restore.count);
        for(int32_t i = 0; i < restore.count; i++)
        {
            printf("SEND PATH %d / %d\n", i, restore.count);
            SendPath(socket, restore.unit[i].path);
        }
    }
}

void Restore_Free(const Restore restore)
{
    free(restore.unit);
}
