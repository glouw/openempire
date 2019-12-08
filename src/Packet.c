#include "Packet.h"

#include "Util.h"

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

Packet Packet_ZeroOverviews(Packet packet)
{
    static Overview zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        packet.overview[i] = zero;
    return packet;
}

bool Packet_IsStable(const Packet packet)
{
    return packet.turn > 0 && packet.is_stable;
}
