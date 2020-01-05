#include "Scanline.h"

#include "Surface.h"
#include "Util.h"

static Scanline New(const int32_t max)
{
    static Scanline zero;
    Scanline scanline = zero;
    scanline.palette_index = UTIL_ALLOC(int32_t, max);
    scanline.count = 0;
    scanline.max = max;
    return scanline;
}

void Scanline_Free(const Scanline scanline)
{
    free(scanline.palette_index);
}

static Scanline Append(Scanline scanline, const int32_t index)
{
    if(scanline.count == scanline.max)
    {
        scanline.max *= 2;
        int32_t* const palette_index = UTIL_REALLOC(scanline.palette_index, int32_t, scanline.max);
        scanline.palette_index = palette_index;
    }
    scanline.palette_index[scanline.count++] = index;
    return scanline;
}

void Scanline_Draw(const Scanline scanline, const int32_t y, const Outline outline, const Palette palette, SDL_Surface* const surface)
{
    for(int32_t x = 0; x < scanline.count; x++)
    {
        const int32_t palette_index = scanline.palette_index[x];
        if(palette_index != -1)
        {
            const uint32_t color = palette.color[palette_index];
            const int32_t xx = x + outline.left_padding;
            Surface_PutPixel(surface, xx, y, color);
        }
    }
}

static uint8_t Feed(uint8_t* const data, uint32_t* index)
{
    const uint8_t get = data[*index];
    *index += 1;
    return get;
}

Scanline Scanline_Get(const Image image, const Frame frame, const int32_t y, const Color color)
{
    const int32_t player_palette_index = 16 * ((int32_t) color + 1);
    Scanline scanline = New(128);
    uint32_t index = image.cmd_table[y];
    for(;;)
    {
        const uint8_t command = Feed(image.data, &index);
        const uint8_t lower_nibble = command & 0x0F;
        const uint8_t upper_nibble = command & 0xF0;

        // END OF SCANLINE
        if(lower_nibble == 0x0F)
            break;

        switch(lower_nibble)
        {
        default:
            Util_Bomb("Regular command 0x%02X not implemented\n", lower_nibble);
            break;

        // LESSER BLOCK COPY
        case 0x0: case 0x4: case 0x8: case 0xC:
        {
            const int32_t count = command >> 2;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, Feed(image.data, &index));
            break;
        }

        // LESSER SKIP
        case 0x1: case 0x5: case 0x9: case 0xD:
        {
            const int32_t shift = command >> 2;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, -1);
            break;
        }

        // GREATER BLOCK COPY
        case 0x2:
        {
            const int32_t count = (upper_nibble << 4) + Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, Feed(image.data, &index));
            break;
        }

        // GREATER SKIP
        case 0x3:
        {
            const int32_t count = (upper_nibble << 4) + Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, -1);
            break;
        }

        // PLAYER COLOR BLOCK COPY
        case 0x6:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            for(int32_t i = 0; i < count; i++)
            {
                const int32_t real_palette_index = Feed(image.data, &index) + player_palette_index;
                scanline = Append(scanline, real_palette_index);
            }
            break;
        }

        // FILL
        case 0x7:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            const uint8_t palette_index = Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, palette_index);
            break;
        }

        // FILL PLAYER COLOR
        case 0xA:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            const int32_t palette_index = Feed(image.data, &index) + player_palette_index;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, palette_index);
            break;
        }

        // DRAW SHADOW
        case 0xB:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, 0);
            break;
        }

        // EXTENDED COMMANDS
        case 0xE:
            switch(command)
            {
            default:
                Util_Bomb("Extended command 0x%02X not implemented\n", command);
                break;

            // RENDER HINT XFLIP
            case 0x0E:
                break;

             // RENDER HINT NOT XFLIP
            case 0x1E:
                break;

            // TABLE USE NORMAL
            case 0x2E:
                break;

            // TABLE USE ALTERNATE
            case 0x3E:
                break;

            // OUTLINE PLAYER COLOR
            case 0x4E:
            {
                scanline = Append(scanline, player_palette_index);
                break;
            }

            // FILL PLAYER COLOR
            case 0x5E:
            {
                const int32_t count = Feed(image.data, &index);
                for(int32_t i = 0; i < count; i++)
                    scanline = Append(scanline, player_palette_index);
                break;
            }

            // OUTLINE PLAYER COLOR 2
            case 0x6E:
            {
                scanline = Append(scanline, 0);
                break;
            }

            // FILL PLAYER COLOR 2
            case 0x7E:
            {
                const int32_t count = Feed(image.data, &index);
                for(int32_t i = 0; i < count; i++)
                    scanline = Append(scanline, 0);
                break;
            }

            // DITHER
            case 0x8E:
                break;

            // PREMULTIPLIED ALPHA
            case 0x9E:
                break;

            // ORIGINAL ALPHA
            case 0xAE:
                break;
            }
        }
        if(scanline.count > frame.width)
            Util_Bomb("Scanline[%d] has grown too large %d of %d\n", y, scanline.count, frame.width);
    }
    return scanline;
}
