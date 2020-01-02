#include "Vram.h"

#include "Surface.h"
#include "Channels.h"
#include "Lines.h"
#include "Config.h"
#include "Util.h"
#include "Rect.h"
#include "Quad.h"
#include "Interfac.h"
#include "Buttons.h"

static Rects GetChannelRects(const int32_t xres, const int32_t yres, const int32_t cpu_count)
{
    Rects rects = Rects_Make(cpu_count);
    const int32_t width = xres / rects.count;
    const int32_t remainder = xres % rects.count;
    for(int32_t i = 0; i < rects.count; i++)
    {
        Rect rect = {
            { (i + 0) * width,    0 },
            { (i + 1) * width, yres },
        };
        if(i == rects.count - 1)
            rect.b.x += remainder;
        rects.rect[i] = rect;
    }
    return rects;
}

Vram Vram_Lock(SDL_Texture* const texture, const int32_t xres, const int32_t yres, const int32_t cpu_count)
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
    vram.cpu_count = cpu_count;
    vram.channel_rects = GetChannelRects(xres, yres, cpu_count);
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

// See: https://gist.github.com/XProger/96253e93baccfbf338de
static uint32_t Blend(const uint32_t bot_pixel, const uint32_t top_pixel, const uint8_t alpha)
{
    uint32_t rb = top_pixel & 0xFF00FF;
    uint32_t g  = top_pixel & 0x00FF00;
    rb += ((bot_pixel & 0xFF00FF) - rb) * alpha >> 8;
    g  += ((bot_pixel & 0x00FF00) -  g) * alpha >> 8;
    return (rb & 0xFF00FF) | (g & 0xFF00);
}

static void TransferTilePixel(const Vram vram, const Tile tile, Point coords, const int32_t x, const int32_t y)
{
    const uint32_t vram_pixel = Get(vram, coords.x, coords.y);
    const uint8_t height = vram_pixel >> SURFACE_ALPHA;
    if(tile.height > height)
    {
        uint32_t surface_pixel = Surface_GetPixel(tile.surface, x, y);
        if(surface_pixel != SURFACE_COLOR_KEY)
        {
            if(height == FILE_PRIO_BUILDING
            || height == FILE_PRIO_SHADOW
            || tile.is_floating)
                surface_pixel = Blend(surface_pixel, vram_pixel, 0xFF / 2);
            const uint32_t pixel = (tile.height << SURFACE_ALPHA) | surface_pixel;
            Put(vram, coords.x, coords.y, pixel);
        }
    }
}

static void DrawTileNoClip(const Vram vram, const Tile tile)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetTopLeftOffsetCoords(tile, x, y);
        TransferTilePixel(vram, tile, coords, x, y);
    }
}

static void DrawTileClip(const Vram vram, const Tile tile)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetTopLeftOffsetCoords(tile, x, y);
        if(Rect_ContainsPoint(tile.bound, coords))
            TransferTilePixel(vram, tile, coords, x, y);
    }
}

void Vram_DrawTile(const Vram vram, const Tile tile)
{
    if(!tile.totally_offscreen)
        tile.needs_clipping ? DrawTileClip(vram, tile) : DrawTileNoClip(vram, tile);
}

typedef struct
{
    Vram vram;
    Tiles tiles;
    int32_t a;
    int32_t b;
}
BatchNeedle;

static int32_t DrawBatchNeedle(void* data)
{
    BatchNeedle* needle = (BatchNeedle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
        Vram_DrawTile(needle->vram, needle->tiles.tile[i]);
    return 0;
}

static void RenderTerrainTiles(const Vram vram, const Tiles terrain_tiles)
{
    BatchNeedle* const needles = UTIL_ALLOC(BatchNeedle, vram.cpu_count);
    const int32_t width = terrain_tiles.count / vram.cpu_count;
    const int32_t remainder = terrain_tiles.count % vram.cpu_count;
    for(int32_t i = 0; i < vram.cpu_count; i++)
    {
        needles[i].vram = vram;
        needles[i].tiles = terrain_tiles;
        needles[i].a = (i + 0) * width;
        needles[i].b = (i + 1) * width;
    }
    needles[vram.cpu_count - 1].b += remainder;
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, vram.cpu_count);
    for(int32_t i = 0; i < vram.cpu_count; i++) threads[i] = SDL_CreateThread(DrawBatchNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < vram.cpu_count; i++) SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
}

static uint32_t BlendMaskWithBuffer(const Vram vram, const int32_t xx, const int32_t yy, SDL_Surface* const mask, const int32_t x, const int32_t y, const uint32_t top_pixel)
{
    const uint32_t mask_pixel = Surface_GetPixel(mask, x, y);
    const uint32_t bot_pixel = Get(vram, xx, yy);
    const uint32_t blend_pixel = Blend(bot_pixel, top_pixel, mask_pixel);
    return blend_pixel;
}

static void BlendTilePixel(const Vram vram, const Tile tile, const Point coords, SDL_Surface* const mask, const int32_t x, const int32_t y)
{
    const uint8_t height = Get(vram, coords.x, coords.y) >> SURFACE_ALPHA;
    if(tile.height >= height) // NOTE: Greater than or equal to so that terrain tiles can blend.
    {
        const uint32_t top_pixel = Surface_GetPixel(tile.surface, x, y);
        if(top_pixel != SURFACE_COLOR_KEY)
        {
            const uint32_t blend_pixel = BlendMaskWithBuffer(vram, coords.x, coords.y, mask, x, y, top_pixel);
            const uint32_t pixel = blend_pixel | (tile.height << SURFACE_ALPHA);
            Put(vram, coords.x, coords.y, pixel);
        }
    }
}

static void DrawTileMaskClip(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetTopLeftOffsetCoords(tile, x, y);
        if(Rect_ContainsPoint(tile.bound, coords))
            BlendTilePixel(vram, tile, coords, mask, x, y);
    }
}

static void DrawTileMaskNoClip(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    for(int32_t y = 0; y < tile.frame.height; y++)
    for(int32_t x = 0; x < tile.frame.width; x++)
    {
        const Point coords = Tile_GetTopLeftOffsetCoords(tile, x, y);
        BlendTilePixel(vram, tile, coords, mask, x, y);
    }
}

static void DrawTileMask(const Vram vram, const Tile tile, SDL_Surface* const mask)
{
    tile.needs_clipping ? DrawTileMaskClip(vram, tile, mask) : DrawTileMaskNoClip(vram, tile, mask);
}

static void DrawBlendLine(const Vram vram, const Line line, const Registrar terrain, const Map map, const Overview overview, const Grid grid, const Blendomatic blendomatic)
{
    const Point inner = line.inner;
    const Point outer = line.outer;
    const Terrain file = Map_GetTerrainFile(map, inner);
    // The outer tile uses the outer tile animation,
    // but with the inner file so that the correct surface can be looked up.
    const Animation outer_animation = terrain.animation[COLOR_GAIA][file];
    const Tile outer_tile = Tile_GetTerrain(overview, grid, outer, outer_animation, file);
    const int32_t index = (file == FILE_FARM) ? 3 : 0;
    const Mode blend_mode = blendomatic.mode[index];
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
    Grid grid;
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
        DrawBlendLine(needle->vram, line, needle->terrain, needle->map, needle->overview, needle->grid, needle->blendomatic);
    }
    return 0;
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

static void BlendTerrainTiles(const Vram vram, const Registrar terrain, const Map map, const Overview overview, const Grid grid, const Lines blend_lines, const Blendomatic blendomatic)
{
    BlendNeedle* const needles = UTIL_ALLOC(BlendNeedle, vram.cpu_count);
    for(int32_t i = 0; i < vram.cpu_count; i++)
    {
        needles[i].vram = vram;
        needles[i].lines = blend_lines;
        needles[i].terrain = terrain;
        needles[i].map = map;
        needles[i].overview = overview;
        needles[i].blendomatic = blendomatic;
        needles[i].grid = grid;
        needles[i].a = GetNextBestBlendTile(blend_lines, i + 0, vram.cpu_count);
        needles[i].b = GetNextBestBlendTile(blend_lines, i + 1, vram.cpu_count);
    }
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, vram.cpu_count);
    for(int32_t i = 0; i < vram.cpu_count; i++) threads[i] = SDL_CreateThread(DrawBlendNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < vram.cpu_count; i++) SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
}

void Vram_DrawMap(const Vram vram, const Registrar terrain, const Map map, const Overview overview, const Grid grid, const Blendomatic blendomatic, const Lines blend_lines, const Tiles terrain_tiles)
{
    RenderTerrainTiles(vram, terrain_tiles);
    if(!overview.event.key_left_shift)
        BlendTerrainTiles(vram, terrain, map, overview, grid, blend_lines, blendomatic);
}

typedef struct
{
    Vram vram;
    Tiles tiles;
}
ChannelNeedle;

static int32_t DrawChannelNeedle(void* data)
{
    ChannelNeedle* const needle = (ChannelNeedle*) data;
    for(int32_t i = 0; i < needle->tiles.count; i++)
        Vram_DrawTile(needle->vram, needle->tiles.tile[i]);
    return 0;
}

void Vram_DrawUnits(const Vram vram, const Tiles tiles)
{
    const Channels channels = Channels_Make(tiles, vram);
    ChannelNeedle* const needles = UTIL_ALLOC(ChannelNeedle, channels.count);
    for(int32_t i = 0; i < channels.count; i++)
    {
        needles[i].vram = vram;
        needles[i].tiles = channels.tiles[i];
    }
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, channels.count);
    for(int32_t i = 0; i < channels.count; i++) threads[i] = SDL_CreateThread(DrawChannelNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < channels.count; i++) SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
    Channels_Free(channels);
}

static void DrawSelectionPixel(const Vram vram, const Point point, const uint32_t color)
{
    if(!OutOfBounds(vram, point.x, point.y))
        if((Get(vram, point.x, point.y) >> SURFACE_ALPHA) < FILE_PRIO_DECAY)
            Put(vram, point.x, point.y, color);
}

// See: https://gist.github.com/bert/1085538
static void DrawEllipse(const Vram vram, Rect rect, const uint32_t color)
{
    int32_t a = abs(rect.b.x - rect.a.x);
    int32_t b = abs(rect.b.y - rect.a.y);
    int32_t c = b & 1;
    int32_t dx = 4 * (1 - a) * b * b;
    int32_t dy = 4 * (1 + c) * a * a;
    int32_t err = dx + dy + c * a * a;
    if(rect.a.x > rect.b.x)
    {
        rect.a.x = rect.b.x;
        rect.b.x += a;
    }
    if(rect.a.y > rect.b.y)
        rect.a.y = rect.b.y;
    rect.a.y += (b + 1) / 2;
    rect.b.y = rect.a.y - c;
    a *= 8 * a;
    c = 8 * b * b;
    do
    {
        const Point point[] = {
            { rect.b.x, rect.a.y },
            { rect.a.x, rect.a.y },
            { rect.a.x, rect.b.y },
            { rect.b.x, rect.b.y },
        };
        for(int32_t i = 0; i < UTIL_LEN(point); i++)
            DrawSelectionPixel(vram, point[i], color);
        const int32_t e2 = 2 * err;
        if(e2 >= dx)
        {
            rect.a.x++;
            rect.b.x--;
            err += dx += c;
        }
        if(e2 <= dy)
        {
            rect.a.y++;
            rect.b.y--;
            err += dy += a;
        }
    }
    while(rect.a.x <= rect.b.x);
    while(rect.a.y - rect.b.y < b)
    {
        const Point point[] = {
            { rect.a.x - 1, rect.a.y },
            { rect.b.x + 1, rect.a.y },
            { rect.a.x - 1, rect.b.y },
            { rect.b.x + 1, rect.b.y },
        };
        for(int32_t i = 0; i < UTIL_LEN(point); i++)
            DrawSelectionPixel(vram, point[i], color);
        rect.a.y++;
        rect.b.y--;
    }
}

void Vram_DrawSelectionBox(const Vram vram, const Overview overview, const uint32_t color, const bool enabled)
{
    if(enabled && Overview_IsSelectionBoxBigEnough(overview))
    {
        const Rect box = Rect_CorrectOrientation(overview.selection_box);
        for(int32_t x = box.a.x; x < box.b.x; x++) Put(vram, x, box.a.y, color);
        for(int32_t x = box.a.x; x < box.b.x; x++) Put(vram, x, box.b.y, color);
        for(int32_t y = box.a.y; y < box.b.y; y++) Put(vram, box.a.x, y, color);
        for(int32_t y = box.a.y; y < box.b.y; y++) Put(vram, box.b.x, y, color);
    }
}

// XXX. INANIMATE objects need a square drawn around them, not an ellipse.
void Vram_DrawUnitSelections(const Vram vram, const Tiles tiles)
{
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        const Point center = Tile_GetHotSpotCoords(tile);
        const Rect rect = Rect_GetEllipse(center, tile.reference->trait.width / CONFIG_GRID_CELL_SIZE);
        if(tile.reference->is_selected)
            DrawEllipse(vram, rect, tile.reference->is_engaged ? 0xFF0000 : 0xFFFFFF);
    }
}

static void DrawHealthBar(const Vram vram, const Point top, const int32_t health, const int32_t max_health, const bool is_inanimate)
{
    const int32_t w = is_inanimate ? 90 : 30;
    const int32_t h = 2;
    const int32_t hh =  h / 2;
    const int32_t hw =  w / 2;
    const int32_t percent = (w * health) / max_health;
    const int32_t y0 = top.y - hh;
    const int32_t y1 = top.y + hh;
    const int32_t x0 = top.x - hw;
    const int32_t x1 = top.x + hw;
    for(int32_t y = y0; y < y1; y++)
    for(int32_t x = x0; x < x1; x++)
        if(!OutOfBounds(vram, x, y))
        {
            const uint32_t base = x - x0 > percent ? 0xFF0000 : 0x00FF00;
            const uint32_t color = base | (FILE_PRIO_HIGHEST << SURFACE_ALPHA);
            Put(vram, x, y, color);
        }
}

void Vram_DrawUnitHealthBars(const Vram vram, const Tiles tiles)
{
    for(int32_t i = 0; i < tiles.count; i++)
    {
        const Tile tile = tiles.tile[i];
        const Point center = Tile_GetHotSpotCoords(tile);
        const Point top = {
            center.x,
            center.y - CONFIG_VRAM_UNIT_HEALTH_HEIGHT,
        };
        Unit* const unit = tile.reference;
        if(unit->is_selected)
            DrawHealthBar(vram, top, unit->health, unit->trait.max_health, unit->trait.is_inanimate);
    }
}

void Vram_DrawMouseTileSelect(const Vram vram, const Registrar terrain, const Overview overview, const Grid grid)
{
    const int32_t line_width = 3;
    const uint32_t color = 0xFFFF0000;
    const Animation animation = terrain.animation[COLOR_GAIA][FILE_DIRT];
    const Image image = animation.image[0];
    const Frame frame = animation.frame[0];
    const Point snap = Overview_IsoSnapTo(overview, grid, overview.mouse_cursor);
    for(int32_t i = 0; i < frame.height; i++)
    {
        const Outline outline = image.outline_table[i];
        const int32_t left  = snap.x + outline.left_padding;
        const int32_t right = snap.x + frame.width - outline.right_padding;
        const int32_t y = i + snap.y;
        for(int32_t j = 0; j < line_width; j++)
        {
            const Point a = { left  - j, y };
            const Point b = { right + j, y };
            DrawSelectionPixel(vram, a, color);
            DrawSelectionPixel(vram, b, color);
        }
    }
}

static void DrawWithBounds(const Vram vram, SDL_Surface* surface, const Point offset, const int32_t y0, const int32_t y1)
{
    for(int32_t y = y0; y < y1; y++)
    for(int32_t x =  0; x < surface->w; x++)
    {
        const uint32_t pixel = Surface_GetPixel(surface, x, y);
        const int32_t xx = offset.x + x;
        const int32_t yy = offset.y + y;
        if(pixel != SURFACE_COLOR_KEY)
            Put(vram, xx, yy, pixel);
    }
}

typedef struct
{
    Animation animation[ICONTYPE_COUNT];
    Buttons buttons;
}
Pack;

static Pack GetPackFromMotive(const Registrar interfac, const Motive motive, const Color color, const Age age)
{
    const Animation* const base = interfac.animation[color];
    static Pack zero;
    Pack pack = zero;
    pack.buttons = Buttons_FromMotive(motive, age);
    pack.animation[ICONTYPE_BUILD] = base[FILE_INTERFAC_BUILDING_ICONS];
    pack.animation[ICONTYPE_UNIT]  = base[FILE_INTERFAC_UNIT_ICONS];
    pack.animation[ICONTYPE_TECH]  = base[FILE_INTERFAC_TECH_ICONS];
    return pack;
}

static void DrawPack(const Vram vram, const Pack pack)
{
    for(int32_t index = 0; index < pack.buttons.count; index++)
    {
        const Button button = pack.buttons.button[index];
        SDL_Surface* const surface = pack.animation[button.icon_type].surface[button.uni.index];
        const Point offset = Point_Layout(index, vram.xres, vram.yres);
        DrawWithBounds(vram, surface, offset, 0, surface->h);
    }
}

void Vram_DrawMotiveRow(const Vram vram, const Registrar interfac, const Motive motive, const Color color, const Age age)
{
    const Pack pack = GetPackFromMotive(interfac, motive, color, age);
    DrawPack(vram, pack);
}

void Vram_DrawHud(const Vram vram, const Registrar interfac)
{
    const int32_t y0 = 0;
    const int32_t y1 = 50;
    const Point corner = { 0, 0 };
    const Animation animation = interfac.animation[COLOR_GAIA][FILE_INTERFAC_HUD_0];
    DrawWithBounds(vram, animation.surface[0], corner, y0, y1);
}

void Vram_Free(const Vram vram)
{
    Rects_Free(vram.channel_rects);
}
