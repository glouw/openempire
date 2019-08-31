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
    UTIL_CHECK(scanline.palette_index);
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
        UTIL_CHECK(palette_index);
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

// XXX. Sometimes scanlines have large streaks of black lines in them.
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

        // End of scanline.
        if(lower_nibble == 0x0F)
            break;

        switch(lower_nibble)
        {
        default:
            Util_Bomb("Regular command 0x%02X not implemented\n", lower_nibble);
            break;

        // Lesser Block Copy.
        case 0x0: case 0x4: case 0x8: case 0xC:
        {
            const int32_t count = command >> 2;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, Feed(image.data, &index));
            break;
        }

        // Lesser Skip.
        case 0x1: case 0x5: case 0x9: case 0xD:
        {
            const int32_t shift = command >> 2;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, -1);
            break;
        }

        // Greater Block Copy.
        case 0x2:
        {
            const int32_t count = (upper_nibble << 4) + Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, Feed(image.data, &index));
            break;
        }

        // Greater Skip.
        case 0x3:
        {
            const int32_t count = (upper_nibble << 4) + Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, -1);
            break;
        }

        // Player Color Block Copy.
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

        // Fill.
        case 0x7:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            Feed(image.data, &index);
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, 0); // Needs to be player color?
            break;
        }

        // Fill Player Color.
        case 0xA:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            const int32_t palette_index = Feed(image.data, &index) + player_palette_index;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, palette_index);
            break;
        }

        // Draw Shadow.
        case 0xB:
        {
            const int32_t shift = command >> 4;
            const int32_t count = (shift == 0) ? Feed(image.data, &index) : shift;
            for(int32_t i = 0; i < count; i++)
                scanline = Append(scanline, 0);
            break;
        }

        // Extended Commands.
        case 0xE:
            switch(command)
            {
            default:
                Util_Bomb("Extended command 0x%02X not implemented\n", command);
                break;

            // Render Hint XFLIP.
            case 0x0E:
                break;

             // Render Hint Not XFLIP.
            case 0x1E:
                break;

            // Table Use Normal.
            case 0x2E:
                break;

            // Table Use Alternate.
            case 0x3E:
                break;

            // Outline Player Color.
            case 0x4E:
            {
                scanline = Append(scanline, player_palette_index);
                break;
            }

            // Fill Player Color.
            case 0x5E:
            {
                const int32_t count = Feed(image.data, &index);
                for(int32_t i = 0; i < count; i++)
                    scanline = Append(scanline, player_palette_index);
                break;
            }

            // Outline Player Color 2.
            case 0x6E:
            {
                scanline = Append(scanline, 0);
                break;
            }

            // Fill Player Color 2.
            case 0x7E:
            {
                const int32_t count = Feed(image.data, &index);
                for(int32_t i = 0; i < count; i++)
                    scanline = Append(scanline, 0);
                break;
            }

            // Dither.
            case 0x8E:
                break;

            // Premultiplied alpha
            case 0x9E:
                break;

            // Original Alpha.
            case 0xAE:
                break;
            }
        }
        if(scanline.count > frame.width)
            Util_Bomb("Scanline[%d] has grown too large %d of %d\n", y, scanline.count, frame.width);
    }
    return scanline;
}
