#include "Packet.h"

Packet Packet_Get(TCPsocket server, SDLNet_SocketSet set)
{
    static Packet zero;
    if(SDLNet_CheckSockets(set, 0))
    {
        if(SDLNet_SocketReady(server))
        {
            Packet packet = zero;
            const int32_t size = sizeof(packet);
            const int32_t bytes = SDLNet_TCP_Recv(server, &packet, size);
            if(bytes == size)
                return packet;
        }
    }
    return zero;
}
