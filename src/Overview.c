#include "Overview.h"

#include "Config.h"

#include <SDL2/SDL.h>

Overview Overview_Init(const int32_t xres, const int32_t yres, const Grid grid)
{
    static Overview zero;
    Overview overview = zero;
    overview.grid = grid;
    overview.xres = xres;
    overview.yres = yres;
    overview.color = Color_GetMyColor();
    return overview;
}

Overview Overview_Update(Overview overview, const Input input)
{
    if(input.key[SDL_SCANCODE_W]) overview.point.y -= CONFIG_OVERVIEW_SCROLL_SPEED;
    if(input.key[SDL_SCANCODE_S]) overview.point.y += CONFIG_OVERVIEW_SCROLL_SPEED;
    if(input.key[SDL_SCANCODE_D]) overview.point.x += CONFIG_OVERVIEW_SCROLL_SPEED;
    if(input.key[SDL_SCANCODE_A]) overview.point.x -= CONFIG_OVERVIEW_SCROLL_SPEED;
    overview.selection_box.a = input.ld_point;
    overview.selection_box.b = input.point;
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
Point Overview_IsoToCart(const Overview overview, const Point iso, const bool raw)
{
    const int32_t x = +iso.x - overview.xres / 2;
    const int32_t y = -iso.y + overview.yres / 2;
    const int32_t xx = x + overview.point.x;
    const int32_t yy = y - overview.point.y;
    const int32_t w = overview.grid.tile_iso_width - 1;
    const int32_t h = overview.grid.tile_iso_height - 1;
    const int32_t rx = (xx * h + yy * w);
    const int32_t ry = (yy * w - xx * h);
    const int32_t cx = (+2 * rx + w * h * overview.grid.cols) / (2 * w);
    const int32_t cy = (-2 * ry + w * h * overview.grid.rows) / (4 * h);
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
Point Overview_CartToIso(const Overview overview, const Point cart)
{
    const int32_t w = overview.grid.tile_iso_width - 1;
    const int32_t h = overview.grid.tile_iso_height - 1;
    const int32_t xx = (cart.y + cart.x) * (w / 2);
    const int32_t yy = (cart.y - cart.x) * (h / 2);
    const int32_t mx = xx + overview.xres / 2;
    const int32_t my = yy + overview.yres / 2;
    const int32_t cx = mx - (w / 2) * overview.grid.cols;
    const int32_t cy = my - (h / 2);
    const Point iso = {
        cx - overview.point.x,
        cy - overview.point.y,
    };
    return iso;
}

Quad Overview_GetRenderBox(const Overview overview, const int32_t border)
{
    const Point p0 = { border, border};
    const Point p1 = { overview.xres - border, border};
    const Point p2 = { border, overview.yres - border };
    const Point p3 = { overview.xres - border, overview.yres - border};
    const Point a = Overview_IsoToCart(overview, p0, false);
    const Point b = Overview_IsoToCart(overview, p1, false);
    const Point c = Overview_IsoToCart(overview, p2, false);
    const Point d = Overview_IsoToCart(overview, p3, false);
    const Quad quad = { a, b, c, d};
    return quad;
}

Point Overview_IsoSnapTo(const Overview overview, const Point iso)
{
    const Point cart = Overview_IsoToCart(overview, iso, false);
    const Point snap = Overview_CartToIso(overview, cart);
    return snap;
}
