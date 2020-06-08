#include "Resource.h"

Resource Resource_None(void)
{
    const Resource none = {
        TYPE_NONE, 0
    };
    return none;
}
