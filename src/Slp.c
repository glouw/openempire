#include "Slp.h"

#include "Util.h"

void Slp_Print(const Slp slp)
{
    Util_Log("version    : %s\n", slp.version);
    Util_Log("num_frames : %d\n", slp.num_frames);
    Util_Log("comment    : %s\n", slp.comment);
    for(uint32_t i = 0; i < slp.num_frames; i++)
    {
        Util_Log("\tFrame %d\n", i);
        Frame_Print(slp.frame[i]);
        Image_Print(slp.image[i]);
    }
}

Slp Slp_Load(const Drs drs, const int32_t table_index, const int32_t file_index)
{
    const Table table = Drs_GetSlpTable(drs, table_index);
    const File file = Table_GetFile(table, file_index);

    // Load SLP header.

    static Slp zero;
    Slp slp = zero;
    fseek(drs.fp, file.data_offset, SEEK_SET);
    UTIL_FREAD(slp.version, sizeof(slp.version) - 1, drs.fp);
    UTIL_FREAD(&slp.num_frames, 1, drs.fp);
    UTIL_FREAD(slp.comment, sizeof(slp.comment) - 1, drs.fp);

    // Load SLP frame headers.

    slp.frame = UTIL_ALLOC(Frame, slp.num_frames);
    UTIL_CHECK(slp.frame);
    for(uint32_t i = 0; i < slp.num_frames; i++)
        slp.frame[i] = Frame_Load(drs.fp);

    // Load SLP frames.

    slp.image = UTIL_ALLOC(Image, slp.num_frames);
    UTIL_CHECK(slp.image);
    for(uint32_t i = 0; i < slp.num_frames; i++)
    {
        // ---------------------- outline_table_offset
        // |         0          |
        // ---------------------- cmd_table_offset
        // |         0          |
        // ---------------------- data_offset
        // |                    |
        // |         0          |
        // |     data_size      |
        // ---------------------- outline_table_offset
        // |         1          |
        // ---------------------- cmd_table_offset
        // |         1          |
        // ---------------------- data_offset
        // |                    |
        // |         1          |
        // |     data_size      |
        // ---------------------- file.size

        static Image dummy;
        const int32_t image_height = slp.frame[i].height;
        const uint32_t data_offset = slp.frame[i].cmd_table_offset + image_height * sizeof(*dummy.cmd_table);
        const bool last_frame = (i == (slp.num_frames - 1));
        const int32_t data_size = last_frame ? (file.size - data_offset) : (slp.frame[i + 1].outline_table_offset - data_offset);
        slp.image[i] = Image_Load(image_height, data_size, drs.fp);
    }
    return slp;
}

void Slp_Free(const Slp slp)
{
    for(uint32_t i = 0; i < slp.num_frames; i++)
        Image_Free(slp.image[i]);
    free(slp.image);
    free(slp.frame);
}
