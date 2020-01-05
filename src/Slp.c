#include "Slp.h"

#include "Util.h"

void Slp_Print(const Slp slp)
{
    fprintf(stderr, "version    : %s\n", slp.version);
    fprintf(stderr, "num_frames : %d\n", slp.num_frames);
    fprintf(stderr, "comment    : %s\n", slp.comment);
    for(uint32_t i = 0; i < slp.num_frames; i++)
    {
        fprintf(stderr, "\tFrame %d\n", i);
        Frame_Print(slp.frame[i]);
        Image_Print(slp.image[i]);
    }
}

// ---------------------- OUTLINE_TABLE_OFFSET
// |         0          |
// ---------------------- CMD_TABLE_OFFSET
// |         0          |
// ---------------------- DATA_OFFSET
// |                    |
// |         0          |
// |     DATA_SIZE      |
// ---------------------- OUTLINE_TABLE_OFFSET
// |         1          |
// ---------------------- CMD_TABLE_OFFSET
// |         1          |
// ---------------------- DATA_OFFSET
// |                    |
// |         1          |
// |     DATA_SIZE      |
// ---------------------- FILE.SIZE
Slp Slp_Load(const Drs drs, const int32_t table_index, const int32_t file_index)
{
    const Table table = Drs_GetSlpTable(drs, table_index);
    const File file = Table_GetFile(table, file_index);
    static Slp zero;
    Slp slp = zero;
    fseek(drs.fp, file.data_offset, SEEK_SET);
    UTIL_FREAD(slp.version, sizeof(slp.version) - 1, drs.fp);
    UTIL_FREAD(&slp.num_frames, 1, drs.fp);
    UTIL_FREAD(slp.comment, sizeof(slp.comment) - 1, drs.fp);
    slp.frame = UTIL_ALLOC(Frame, slp.num_frames);
    for(uint32_t i = 0; i < slp.num_frames; i++)
        slp.frame[i] = Frame_Load(drs.fp);
    slp.image = UTIL_ALLOC(Image, slp.num_frames);
    for(uint32_t i = 0; i < slp.num_frames; i++)
    {
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
