#include "Sockets.h"

#include "Packet.h"
#include "Util.h"

#include <stdlib.h>
#include <stdbool.h>

Sockets Sockets_Init(void)
{
    static Sockets zero;
    Sockets sockets = zero;
    sockets.set = SDLNet_AllocSocketSet(COLOR_COUNT);
    return sockets;
}

void Sockets_Free(const Sockets sockets)
{
    SDLNet_FreeSocketSet(sockets.set);
}

Sockets Sockets_Add(Sockets sockets, TCPsocket sock)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        if(sockets.sock[i] == NULL)
        {
            SDLNet_TCP_AddSocket(sockets.set, sock);
            sockets.sock[i] = sock;
            return sockets;
        }
    return sockets;
}

Sockets Sockets_Service(Sockets sockets, const int32_t timeout)
{
    if(SDLNet_CheckSockets(sockets.set, timeout))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            TCPsocket sock = sockets.sock[i];
            if(SDLNet_SocketReady(sock))
            {
                static Overview zero;
                Overview overview = zero;
                const int32_t bytes = SDLNet_TCP_Recv(sock, &overview, sizeof(overview));
                if(bytes == 0)
                {
                    SDLNet_TCP_DelSocket(sockets.set, sock);
                    sockets.sock[i] = NULL;
                }
                else
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
        printf("%d\n", cycles);
        return Clear(sockets);
    }
    return sockets;
}

Sockets Sockets_Accept(const Sockets sockets, const TCPsocket server)
{
    const TCPsocket client = SDLNet_TCP_Accept(server);
    return (client != NULL)
        ? Sockets_Add(sockets, client)
        : sockets;
}
