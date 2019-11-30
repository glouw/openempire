#include "Sockets.h"

#include "Util.h"

#include <stdlib.h>
#include <stdbool.h>

Sockets Sockets_Init(const int32_t port)
{
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);
    static Sockets zero;
    Sockets sockets = zero;
    sockets.self = SDLNet_TCP_Open(&ip);
    sockets.set = SDLNet_AllocSocketSet(COLOR_COUNT);
    return sockets;
}

void Sockets_Free(const Sockets sockets)
{
    SDLNet_TCP_Close(sockets.self);
    SDLNet_FreeSocketSet(sockets.set);
}

static Sockets Add(Sockets sockets, TCPsocket socket)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        if(sockets.socket[i] == NULL)
        {
            SDLNet_TCP_AddSocket(sockets.set, socket);
            sockets.socket[i] = socket;
            return sockets;
        }
    return sockets;
}

Sockets Sockets_Service(Sockets sockets, const int32_t timeout)
{
    if(SDLNet_CheckSockets(sockets.set, timeout))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            TCPsocket socket = sockets.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                static Overview zero;
                Overview overview = zero;
                const int32_t max = sizeof(overview);
                const int32_t bytes = SDLNet_TCP_Recv(socket, &overview, max);
                if(bytes <= 0)
                {
                    SDLNet_TCP_DelSocket(sockets.set, socket);
                    sockets.socket[i] = NULL;
                }
                if(bytes == max && Overview_UsedAction(overview))
                    sockets.packet.overview[i] = overview;
            }
        }
    return sockets;
}

static Sockets Clear(Sockets sockets)
{
    static Overview zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        sockets.packet.overview[i] = zero;
    return sockets;
}

Sockets Sockets_Relay(const Sockets sockets, const int32_t cycles, const int32_t interval)
{
    if((cycles % interval) == 0)
    {
        printf("%10d :: ", cycles);
        for(int32_t i = 0; i < COLOR_COUNT; i++)
            printf("%d ", sockets.socket[i] == NULL ? 0 : 1);
        putchar('\n');
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            TCPsocket socket = sockets.socket[i];
            if(socket)
                SDLNet_TCP_Send(socket, &sockets.packet, sizeof(sockets.packet));
        }
        return Clear(sockets);
    }
    return sockets;
}

Sockets Sockets_Accept(const Sockets sockets)
{
    const TCPsocket client = SDLNet_TCP_Accept(sockets.self);
    return (client != NULL)
        ? Add(sockets, client)
        : sockets;
}
