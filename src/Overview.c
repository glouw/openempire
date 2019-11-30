#include "Overview.h"

#include "Config.h"
#include "Util.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

Overview Overview_Init(const Color color, const int32_t xres, const int32_t yres)
{
    static Overview zero;
    Overview overview = zero;
    overview.xres = xres;
    overview.yres = yres;
    overview.color = color;
    return overview;
}

static Overview UpdateMouse(Overview overview, const Input input)
{
    overview.mouse_cursor = input.cursor;
    overview.mouse_l = input.l;
    overview.mouse_r = input.r;
    overview.mouse_ld = input.ld;
    overview.mouse_lu = input.lu;
    overview.mouse_rd = input.rd;
    overview.mouse_ru = input.ru;
    return overview;
}

static Overview UpdateKeys(Overview overview, const Input input)
{
    overview.key_left_shift = input.key[SDL_SCANCODE_LSHIFT];
    overview.key_q = input.key[SDL_SCANCODE_Q];
    overview.key_w = input.key[SDL_SCANCODE_W];
    overview.key_e = input.key[SDL_SCANCODE_E];
    overview.key_r = input.key[SDL_SCANCODE_R];
    overview.key_t = input.key[SDL_SCANCODE_T];
    overview.key_a = input.key[SDL_SCANCODE_A];
    overview.key_s = input.key[SDL_SCANCODE_S];
    overview.key_d = input.key[SDL_SCANCODE_D];
    overview.key_f = input.key[SDL_SCANCODE_F];
    overview.key_g = input.key[SDL_SCANCODE_G];
    overview.key_z = input.key[SDL_SCANCODE_Z];
    overview.key_x = input.key[SDL_SCANCODE_X];
    overview.key_c = input.key[SDL_SCANCODE_C];
    overview.key_v = input.key[SDL_SCANCODE_V];
    overview.key_b = input.key[SDL_SCANCODE_B];
    overview.key_1 = input.key[SDL_SCANCODE_1];
    overview.key_2 = input.key[SDL_SCANCODE_2];
    overview.key_3 = input.key[SDL_SCANCODE_3];
    overview.key_left_ctrl = input.key[SDL_SCANCODE_LCTRL];
    return overview;
}

static Overview UpdateSelectionBox(Overview overview)
{
    if(overview.mouse_ld)
        overview.selection_box.a = overview.mouse_cursor;
    overview.selection_box.b = overview.mouse_cursor;
    return overview;
}

static Overview UpdatePan(Overview overview)
{
    if(!overview.key_left_shift)
    {
        if(overview.key_w) overview.pan.y -= CONFIG_OVERVIEW_SCROLL_SPEED;
        if(overview.key_s) overview.pan.y += CONFIG_OVERVIEW_SCROLL_SPEED;
        if(overview.key_d) overview.pan.x += CONFIG_OVERVIEW_SCROLL_SPEED;
        if(overview.key_a) overview.pan.x -= CONFIG_OVERVIEW_SCROLL_SPEED;
    }
    return overview;
}

Overview Overview_Update(Overview overview, const Input input)
{
    overview = UpdateMouse(overview, input);
    overview = UpdateKeys(overview, input);
    overview = UpdatePan(overview);
    overview = UpdateSelectionBox(overview);
    return overview;
}

bool Overview_IsSelectionBoxBigEnough(const Overview overview)
{
    return Rect_GetArea(overview.selection_box) > CONFIG_OVERVIEW_SELECTION_BOX_MIN_SIZE;
}

/*      +
 *     /d\
 *    /c h\      +--------+
 *   /b g l\     |a b c d |
 *  +a f k p+ -> |e f g h |
 *   \e j o/     |i j k l |
 *    \i n/      |m n o p |
 *     \m/       +--------+
 *      +
 *
 * This isometric to cartesian projection preserves integer rounding errors.
 */
Point Overview_IsoToCart(const Overview overview, const Grid grid, const Point iso, const bool raw)
{
    const int32_t x = +iso.x - overview.xres / 2;
    const int32_t y = -iso.y + overview.yres / 2;
    const int32_t xx = x + overview.pan.x;
    const int32_t yy = y - overview.pan.y;
    const int32_t w = grid.tile_iso_width - 1;
    const int32_t h = grid.tile_iso_height - 1;
    const int32_t rx = (xx * h + yy * w);
    const int32_t ry = (yy * w - xx * h);
    const int32_t cx = (+2 * rx + w * h * grid.cols) / (2 * w);
    const int32_t cy = (-2 * ry + w * h * grid.rows) / (4 * h);
    const Point cart_raw = { cx, cy };
    const Point cart = {
        1 * cx / h,
        2 * cy / w,
    };
    return raw ? cart_raw : cart;
}

/*                     +
 *                    /d\
 *  +--------+       /c h\
 *  |a b c d |      /b g l\
 *  |e f g h | --> +a f k p+
 *  |i j k l |      \e j o/
 *  |m n o p |       \i n/
 *  +--------+        \m/
 *                     +
 */
Point Overview_CartToIso(const Overview overview, const Grid grid, const Point cart)
{
    const int32_t w = grid.tile_iso_width - 1;
    const int32_t h = grid.tile_iso_height - 1;
    const int32_t xx = (cart.y + cart.x) * (w / 2);
    const int32_t yy = (cart.y - cart.x) * (h / 2);
    const int32_t mx = xx + overview.xres / 2;
    const int32_t my = yy + overview.yres / 2;
    const int32_t cx = mx - (w / 2) * grid.cols;
    const int32_t cy = my - (h / 2);
    const Point iso = {
        cx - overview.pan.x,
        cy - overview.pan.y,
    };
    return iso;
}

Quad Overview_GetRenderBox(const Overview overview, const Grid grid, const int32_t border)
{
    const Point p0 = { border, border};
    const Point p1 = { overview.xres - border, border};
    const Point p2 = { border, overview.yres - border };
    const Point p3 = { overview.xres - border, overview.yres - border};
    const Point a = Overview_IsoToCart(overview, grid, p0, false);
    const Point b = Overview_IsoToCart(overview, grid, p1, false);
    const Point c = Overview_IsoToCart(overview, grid, p2, false);
    const Point d = Overview_IsoToCart(overview, grid, p3, false);
    const Quad quad = { a, b, c, d};
    return quad;
}

Point Overview_IsoSnapTo(const Overview overview, const Grid grid, const Point iso)
{
    const Point cart = Overview_IsoToCart(overview, grid, iso, false);
    const Point snap = Overview_CartToIso(overview, grid, cart);
    return snap;
}

bool Overview_UsedAction(const Overview overview)
{
    return overview.mouse_lu || overview.mouse_ru;
}
