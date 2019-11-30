#include "Packet.h"

Packet Packet_Get(const Sock sock)
{
    static Packet zero;
    if(SDLNet_CheckSockets(sock.set, 0))
    {
        if(SDLNet_SocketReady(sock.server))
        {
            Packet packet = zero;
            const int32_t size = sizeof(packet);
            const int32_t bytes = SDLNet_TCP_Recv(sock.server, &packet, size);
            if(bytes == size)
                return packet;
        }
    }
    return zero;
}
