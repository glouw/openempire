#include "Frame.h"

#include "Surface.h"
#include "Util.h"

#include <assert.h>

Frame Frame_Load(FILE* const fp)
{
    static Frame zero;
    Frame frame = zero;
    UTIL_FREAD(&frame.cmd_table_offset, 1, fp);
    UTIL_FREAD(&frame.outline_table_offset, 1, fp);
    UTIL_FREAD(&frame.palette_offset, 1, fp);
    UTIL_FREAD(&frame.properties, 1, fp);
    UTIL_FREAD(&frame.width, 1, fp);
    UTIL_FREAD(&frame.height, 1, fp);
    UTIL_FREAD(&frame.hotspot_x, 1, fp);
    UTIL_FREAD(&frame.hotspot_y, 1, fp);
    return frame;
}

void Frame_Print(const Frame frame)
{
    Util_Log("\t\tcmd_table_offset     : %d\n", frame.cmd_table_offset);
    Util_Log("\t\toutline_table_offset : %d\n", frame.outline_table_offset);
    Util_Log("\t\tpalette_offset       : %d\n", frame.palette_offset);
    Util_Log("\t\tproperties           : %d\n", frame.properties);
    Util_Log("\t\twidth                : %d\n", frame.width);
    Util_Log("\t\theight               : %d\n", frame.height);
    Util_Log("\t\thotspot_x            : %d\n", frame.hotspot_x);
    Util_Log("\t\thotspot_y            : %d\n", frame.hotspot_y);
}
