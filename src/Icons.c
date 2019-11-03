#include "Icons.h"

#include <stdlib.h>

Icons Icons_FromMotive(const Motive motive, const int32_t age)
{
    static Icons zero;
    Icons icons = zero;
    icons.icon = NULL;
    icons.count = 0;
    switch(motive.action)
    {
    case ACTION_BUILD:
        icons.icon = Icon_GetBuilding(age);
        icons.count = Icon_GetBuildingLen(age);
        break;
    case ACTION_UNIT_TECH:
        switch(motive.type)
        {
        case TYPE_BARRACKS:
            icons.icon = Icon_GetBarracks(age);
            icons.count = Icon_GetBarracksLen(age);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return icons;
}
