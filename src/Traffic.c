#include "Traffic.h"

#include "Util.h"

void Traffic_Print(const Traffic traffic)
{
    printf("%5d :: ", traffic.setpoint);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        printf("%5d ", traffic.cycles[i]);
    putchar('\n');
}
