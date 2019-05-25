#include "Vram.h"

#include "Surface.h"
#include "Lines.h"
#include "Util.h"
#include "Quad.h"

Vram Vram_Lock(SDL_Texture* const texture, const int32_t xres, const int32_t yres)
{
    void* raw;
    int32_t pitch;
    SDL_LockTexture(texture, NULL, &raw, &pitch);
    static Vram zero;
    Vram vram = zero;
    vram.pixels = (uint32_t*) raw;
    vram.width = (int32_t) (pitch / sizeof(*vram.pixels));
    vram.xres = xres;
    vram.yres = yres;
    vram.cpu_count = 2 * SDL_GetCPUCount(); // XXX. Need benchmarks to find best ratio.
    return vram;
}

void Vram_Unlock(SDL_Texture* const texture)
{
    SDL_UnlockTexture(texture);
}

static void Put(const Vram vram, const int32_t x, const int32_t y, const uint32_t pixel)
{
    vram.pixels[x + y * vram.width] = pixel;
}

static uint32_t Get(const Vram vram, const int32_t x, const int32_t y)
{
    return vram.pixels[x + y * vram.width];
}

void Vram_DrawCross(const Vram vram, const Point point, const int32_t len, const uint32_t color)
{
    for(int32_t x = point.x - len; x <= point.x + len; x++) Put(vram, x, point.y, color);
    for(int32_t y = point.y - len; y <= point.y + len; y++) Put(vram, point.x, y, color);
}

void Vram_Clear(const Vram vram, const uint32_t color)
{
    for(int32_t y = 0; y < vram.yres; y++)
    for(int32_t x = 0; x < vram.xres; x++)
        Put(vram, x, y, color);
}

static bool OutOfBounds(const Vram vram, const int32_t x, const int32_t y)
{
    return x < 0 || y < 0 || x >= vram.xres || y >= vram.yres;
}

static void Transfer(const Vram vram, const Tile tile, const Point coords, const int32_t x, const int32_t y)
{
    const uint8_t height = Get(vram, coords.x, coords.y) >> 24;
    if(tile.height > height)
    {
        const uint32_t surface_pixel = Surface_GetPixel(tile.surface, x, y);
        if(surface_pixel != SURFACE_COLOR_KEY)
        {
            const uint32_t pixel = (tile.height << 24) | surface_pixel;
            Put(vram, coords.x, coords.y, pixel);
        }
    }
}

static void DrawTileNoClip(const Vram vram, const Tile tile)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetScreenCoords(tile, x, y);
        Transfer(vram, tile, coords, x, y);
    }
}

static void DrawTileClip(const Vram vram, const Tile tile)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetScreenCoords(tile, x, y);
        if(!OutOfBounds(vram, coords.x, coords.y))
            Transfer(vram, tile, coords, x, y);
    }
}

void Vram_DrawTile(const Vram vram, const Tile tile)
{
    tile.needs_clipping ? DrawTileClip(vram, tile) : DrawTileNoClip(vram, tile);
}

typedef struct
{
    Vram vram;
    Tile* tiles;
    int32_t a;
    int32_t b;
}
BatchNeedle;

static int32_t DrawBatchNeedle(void* data)
{
    BatchNeedle* needle = (BatchNeedle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
        Vram_DrawTile(needle->vram, needle->tiles[i]);
    return 0;
}

static void RenderTerrainTiles(const Vram vram, const Registrar terrain, const Map map, const Overview overview, Points points)
{
    const Tiles tiles = Tiles_PrepTerrain(terrain, map, overview, points);
    BatchNeedle* const needles = UTIL_ALLOC(BatchNeedle, vram.cpu_count);
    UTIL_CHECK(needles);
    for(int32_t i = 0; i < vram.cpu_count; i++)
    {
        const int32_t width = points.count / vram.cpu_count;
        const int32_t remainder = points.count % vram.cpu_count;
        needles[i].vram = vram;
        needles[i].tiles = tiles.tile;
        needles[i].a = (i + 0) * width;
        needles[i].b = (i + 1) * width;
        if(i == vram.cpu_count - 1)
            needles[i].b += remainder;
    }
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, vram.cpu_count);
    UTIL_CHECK(threads);
    for(int32_t i = 0; i < vram.cpu_count; i++)
        threads[i] = SDL_CreateThread(DrawBatchNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < vram.cpu_count; i++)
        SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
    Tiles_Free(tiles);
}

// See:
//   https://gist.github.com/XProger/96253e93baccfbf338de

static uint32_t Blend(const uint32_t bot_pixel, const uint32_t top_pixel, const uint8_t alpha)
{
    uint32_t rb = top_pixel & 0xFF00FF;
    uint32_t g  = top_pixel & 0x00FF00;
    rb += ((bot_pixel & 0xFF00FF) - rb) * alpha >> 8;
    g  += ((bot_pixel & 0x00FF00) -  g) * alpha >> 8;
    return (rb & 0xFF00FF) | (g & 0xFF00);
}

static uint32_t BlendMaskWithBuffer(const Vram vram, const int32_t xx, const int32_t yy, SDL_Surface* const mask, const int32_t x, const int32_t y, const uint32_t top_pixel)
{
    const uint32_t mask_pixel = Surface_GetPixel(mask, x, y);
    const uint32_t bot_pixel = Get(vram, xx, yy);
    const uint32_t blend_pixel = Blend(bot_pixel, top_pixel, mask_pixel);
    return blend_pixel;
}

static void BlendTransfer(const Vram vram, const Tile tile, const Point coords, SDL_Surface* const mask, const int32_t x, const int32_t y)
{
    const uint8_t height = Get(vram, coords.x, coords.y) >> 24;
    if(tile.height >= height) // NOTE: Greater than or equal to so that terrain tiles can blend.
    {
        const uint32_t top_pixel = Surface_GetPixel(tile.surface, x, y);
        if(top_pixel != SURFACE_COLOR_KEY)
        {
            const uint32_t blend_pixel = BlendMaskWithBuffer(vram, coords.x, coords.y, mask, x, y, top_pixel);
            const uint32_t pixel = blend_pixel | (tile.height << 24);
            Put(vram, coords.x, coords.y, pixel);
        }
    }
}

static void DrawTileMaskClip(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetScreenCoords(tile, x, y);
        if(!OutOfBounds(vram, coords.x, coords.y))
            BlendTransfer(vram, tile, coords, mask, x, y);
    }
}

static void DrawTileMaskNoClip(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetScreenCoords(tile, x, y);
        BlendTransfer(vram, tile, coords, mask, x, y);
    }
}

static void DrawTileMask(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    tile.needs_clipping ? DrawTileMaskClip(vram, tile, mask) : DrawTileMaskNoClip(vram, tile, mask);
}

static void DrawBlendLine(const Vram vram, const Line line, const Registrar terrain, const Map map, const Overview overview, const Blendomatic blendomatic)
{
    const Point inner = line.inner;
    const Point outer = line.outer;
    const Terrain inner_tile_file = Map_GetTerrainFile(map, inner);

    // The outer tile uses the outer tile animation,
    // but with the inner tile_file so that the correct surface can be looked up.

    const Animation outer_animation = terrain.animation[COLOR_BLU][inner_tile_file];
    const Tile outer_tile = Tile_GetTerrain(overview, outer, outer_animation, inner_tile_file);

    const Mode blend_mode = blendomatic.mode[0]; // XXX: Which mode? How to choose?
    const int32_t blend_id = Mode_GetBlendIndex(inner, outer);
    DrawTileMask(vram, outer_tile, blend_mode.mask_real[blend_id]);
}

typedef struct
{
    Vram vram;
    Lines lines;
    Registrar terrain;
    Map map;
    Overview overview;
    Blendomatic blendomatic;
    int32_t a;
    int32_t b;
}
BlendNeedle;

static int32_t DrawBlendNeedle(void* const data)
{
    BlendNeedle* const needle = (BlendNeedle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        const Line line = needle->lines.line[i];
        DrawBlendLine(needle->vram, line, needle->terrain, needle->map, needle->overview, needle->blendomatic);
    }
    return 0;
}

static Lines AppendBlendLines(Lines lines, const Map map, const Point inner)
{
    const Points box = Map_GetBlendBox(map, inner);
    for(int32_t j = 0; j < box.count; j++)
    {
        // Outer tile is partially transparent blended tile.
        // Inner tile is a solid inner tile.

        const Terrain tile_file = Map_GetTerrainFile(map, inner);
        const Point outer = box.point[j];
        const Line line = { inner, outer, tile_file };
        lines = Lines_Append(lines, line);
    }
    Points_Free(box);
    return lines;
}

// Blend lines indicate tile blending by direction.

static Lines GetBlendLines(const Map map, const Points points)
{
    Lines lines = Lines_New(8 * points.count);
    for(int32_t i = 0; i < points.count; i++)
    {
        const Point inner = points.point[i];
        lines = AppendBlendLines(lines, map, inner);
    }
    return lines;
}

static int32_t GetNextBestBlendTile(const Lines lines, const int32_t slice, const int32_t slices)
{
    if(slice == 0)
        return 0;
    const int32_t width = slice * lines.count / slices;
    int32_t index = width;
    while(index < lines.count)
    {
        // Since lines are sorted by outer tiles, an outer tile may get shared
        // across threads. To counteract that, advance the index of the end of the slice
        // to a new outer blend tile.

        const Point prev = lines.line[index - 1].outer;
        const Point curr = lines.line[index - 0].outer;
        if(Point_Equal(prev, curr))
        {
            index++;
            continue;
        }
        else break;
    }
    return index;
}

static void BlendTerrainTiles(const Vram vram, const Registrar terrain, const Map map, const Overview overview, const Points points, const Blendomatic blendomatic)
{
    const Lines lines = GetBlendLines(map, points);
    Lines_Sort(lines);
    BlendNeedle* const needles = UTIL_ALLOC(BlendNeedle, vram.cpu_count);
    UTIL_CHECK(needles);
    for(int32_t i = 0; i < vram.cpu_count; i++)
    {
        needles[i].vram = vram;
        needles[i].lines = lines;
        needles[i].terrain = terrain;
        needles[i].map = map;
        needles[i].overview = overview;
        needles[i].blendomatic = blendomatic;
        needles[i].a = GetNextBestBlendTile(lines, i + 0, vram.cpu_count);
        needles[i].b = GetNextBestBlendTile(lines, i + 1, vram.cpu_count);
    }
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, vram.cpu_count);
    UTIL_CHECK(threads);
    for(int32_t i = 0; i < vram.cpu_count; i++)
        threads[i] = SDL_CreateThread(DrawBlendNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < vram.cpu_count; i++)
        SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
    Lines_Free(lines);
}

void Vram_DrawMap(const Vram vram, const Registrar terrain, const Map map, const Overview overview, const Blendomatic blendomatic, const Input input)
{
    const Quad quad = Overview_GetRenderBox(overview, -2 * map.tile_width); // XXX: Should this really be twice the width?
    const Points points = Quad_GetRenderPoints(quad);
    RenderTerrainTiles(vram, terrain, map, overview, points);
    if(!input.key[SDL_SCANCODE_LSHIFT])
        BlendTerrainTiles(vram, terrain, map, overview, points, blendomatic);
    Points_Free(points);
}

void Vram_DrawUnits(const Vram vram, const Registrar graphics, const Units units, const Overview overview)
{
    const Quad quad = Overview_GetRenderBox(overview, -200); // XXX: Border needs to be equal to largest building size.
    const Points points = Quad_GetRenderPoints(quad);
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, points);
    for(int32_t i = 0; i < tiles.count; i++)
        Vram_DrawTile(vram, tiles.tile[i]);
    Tiles_Free(tiles);
    Points_Free(points);
}

static void DrawSelectionPixel(const Vram vram, const int32_t x, const int32_t y, const uint32_t color)
{
    if(!OutOfBounds(vram, x, y))
        if((Get(vram, x, y) >> 24) < FILE_PRIO_GRAPHICS)
            Put(vram, x, y, color);
}

void Vram_DrawMouseTileSelect(const Vram vram, const Registrar terrain, const Input input, const Overview overview)
{
    const int32_t line_width = 3;
    const uint32_t color = 0xFFFF0000;
    const Image image = terrain.animation[COLOR_BLU][FILE_DIRT].image[0];
    const Frame frame = terrain.animation[COLOR_BLU][FILE_DIRT].frame[0];
    const Point iso = { input.x, input.y };
    const Point snap = Overview_IsoSnapTo(overview, iso);

    // The real tile width and height is one less to allow for pixel overlaps.
    // Look at Overview.c IsoToCart and CartToIso for more information.

    const int32_t width = frame.width - 1;
    const int32_t height = frame.height - 1;

    for(int32_t i = 0; i < height; i++)
    {
        const Outline outline = image.outline_table[i];
        const int32_t left  = snap.x + outline.left_padding;
        const int32_t right = snap.x + width - outline.right_padding;
        const int32_t y = i + snap.y;
        for(int32_t j = 0; j < line_width; j++)
        {
            const int32_t xl = left - j;
            const int32_t xr = right + j;
            DrawSelectionPixel(vram, xl, y, color);
            DrawSelectionPixel(vram, xr, y, color);
        }
    }
}

static void Draw4Points(const Vram vram, const Point center, const int32_t x, const int32_t y, const uint32_t pixel)
{
    DrawSelectionPixel(vram, center.x + x, center.y + y, pixel);
    if(x != 0)
        DrawSelectionPixel(vram, center.x - x, center.y + y, pixel);
    if(y != 0)
        DrawSelectionPixel(vram, center.x + x, center.y - y, pixel);
    if(x != 0 && y != 0)
        DrawSelectionPixel(vram, center.x - x, center.y - y, pixel);
}

static void Draw8Points(const Vram vram, const Point center, const int32_t x, const int32_t y, const uint32_t pixel)
{
    Draw4Points(vram, center, x, y, pixel);
    if(x != y)
        Draw4Points(vram, center, y, x, pixel);
} 

void Vram_DrawCircle(const Vram vram, const Point center, const int32_t radius, const uint32_t pixel)
{
    int32_t error = -radius;
    int32_t x = radius;
    int32_t y = 0;
    while(x >= y)
    {
        Draw8Points(vram, center, x, y, pixel);
        error += y;
        ++y;
        error += y;
        if(error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
    }
}
