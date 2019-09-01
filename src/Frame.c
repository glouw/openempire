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
    fprintf(stderr, "\t\tcmd_table_offset     : %d\n", frame.cmd_table_offset);
    fprintf(stderr, "\t\toutline_table_offset : %d\n", frame.outline_table_offset);
    fprintf(stderr, "\t\tpalette_offset       : %d\n", frame.palette_offset);
    fprintf(stderr, "\t\tproperties           : %d\n", frame.properties);
    fprintf(stderr, "\t\twidth                : %d\n", frame.width);
    fprintf(stderr, "\t\theight               : %d\n", frame.height);
    fprintf(stderr, "\t\thotspot_x            : %d\n", frame.hotspot_x);
    fprintf(stderr, "\t\thotspot_y            : %d\n", frame.hotspot_y);
}
