#include "Image.h"

#include "Scanline.h"
#include "Surface.h"
#include "Util.h"

#include <string.h>

static Image New(const int32_t height, const int32_t size)
{
    static Image zero;
    Image image = zero;
    image.outline_table = UTIL_ALLOC(Outline, height);
    image.cmd_table = UTIL_ALLOC(uint32_t, height);
    image.data = UTIL_ALLOC(uint8_t, size);
    image.height = height;
    image.size = size;
    return image;
}

Image Image_Copy(const Image image)
{
    Image copy = New(image.height, image.size);
    for(int32_t i = 0; i < image.height; i++)
    {
        copy.outline_table[i] = image.outline_table[i];
        copy.cmd_table[i] = image.cmd_table[i];
    }
    for(int32_t i = 0; i < image.size; i++)
        copy.data[i] = image.data[i];
    return copy;
}

Image Image_Load(const int32_t height, const int32_t size, FILE* const fp)
{
    Image image = New(height, size);
    const int32_t unpadded_outline_size = Outline_GetUnpaddedSize();
    fread(image.outline_table, unpadded_outline_size, height, fp);
    UTIL_FREAD(image.cmd_table, height, fp);
    UTIL_FREAD(image.data, size, fp);
    const uint32_t base = image.cmd_table[0];
    for(int32_t i = 0; i < height; i++)
        image.cmd_table[i] -= base;
    return image;
}

void Image_Print(const Image image)
{
    fprintf(stderr, "\t\t\tpadding:\n");
    for(int32_t i = 0; i < image.height; i++)
    {
        const bool transparent = Outline_IsTransparent(image.outline_table[i]);
        fprintf(stderr, "\t\t\t\t%3d: transparent = %d\n", i, transparent);
    }
    fprintf(stderr, "\t\t\tcmd_table:\n");
    for(int32_t i = 0; i < image.height; i++)
    {
        const uint32_t address = image.cmd_table[i];
        const uint8_t cmd = image.data[address];
        fprintf(stderr, "\t\t\t\t%3d: 0x%08X -> 0x%02X\n", i, address, cmd);
    }
}

void Image_Free(const Image image)
{
    free(image.outline_table);
    free(image.cmd_table);
    free(image.data);
}

SDL_Surface* Image_Parse(const Image image, const Frame frame, const Palette palette, const Color color)
{
    SDL_Surface* const surface = Surface_Make(frame.width, frame.height);
    for(int32_t y = 0; y < image.height; y++)
    {
        const Outline outline = image.outline_table[y];
        if(Outline_IsTransparent(outline))
            continue;
        const Scanline scanline = Scanline_Get(image, frame, y, color);
        Scanline_Draw(scanline, y, outline, palette, surface);
        Scanline_Free(scanline);
    }
    return surface;
}
