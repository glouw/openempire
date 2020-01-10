#include "Traffic.h"

Traffic Traffic_Make(const Packet packet)
{
    static Traffic zero;
    Traffic traffic = zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        traffic.cycles[i] = packet.overview[i].cycles;
    traffic.setpoint = packet.setpoint;
    return traffic;
}
