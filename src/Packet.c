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
            const int32_t bytes = UTIL_TCP_RECV_NO_ASSERT(sock.server, &packet);
            if(bytes == sizeof(packet))
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

bool Packet_IsAlive(const Packet packet)
{
    return packet.turn > 0;
}

bool Packet_IsReady(const Packet packet)
{
    return Packet_IsAlive(packet) && packet.is_stable;
}

void Packet_Flush(const Sock sock)
{
    while(Packet_Get(sock).turn != 0);
}
