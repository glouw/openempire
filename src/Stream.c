#include "Stream.h"

#include "Util.h"

Stream Stream_Init(void)
{
    static Stream zero;
    Stream stream = zero;
    return stream;
}

Stream Stream_Flow(Stream stream, const Sock sock)
{
    const Packet packet = Packet_Get(sock);
    if(packet.turn > 0)
    {
        if(stream.turn > 0)
        {
            const int32_t delta = packet.turn - stream.turn;
            if(delta != 1)
                Util_Bomb("OUT OF SYNC - CLIENT MISSED A PACKET\n");
        }
        stream.turn = packet.turn;
    }
    stream.packet = packet;
    return stream;
}
