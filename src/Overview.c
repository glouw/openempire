#include "Overview.h"

#include <SDL2/SDL.h>

Overview Overview_Init(const int32_t xres, const int32_t yres, const Grid grid)
{
    static Overview zero;
    Overview overview = zero;
    overview.grid = grid;
    overview.xres = xres;
    overview.yres = yres;
    return overview;
}

Overview Overview_Update(Overview overview, const Input input)
{
    if(input.key[SDL_SCANCODE_W]) overview.point.y += 15; // XXX: Maybe add acceleration?
    if(input.key[SDL_SCANCODE_S]) overview.point.y -= 15;
    if(input.key[SDL_SCANCODE_D]) overview.point.x += 15;
    if(input.key[SDL_SCANCODE_A]) overview.point.x -= 15;
    return overview;
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
 * This isometric to cartesian projection is a little long winded,
 * where are purposely not canceled to preserve integer rounding error.
 *
 * If the raw flag is set to true, the returned cartesian projection granularity will be
 * of screen pixel width.
 *
 * If the raw flag is set to false, the returned granularity is that of the tile dimensions.
 *
 */

Point Overview_IsoToCart(const Overview overview, const Point iso, const bool raw)
{
    // Relative to middle of screen.

    const int32_t x = +iso.x - overview.xres / 2;
    const int32_t y = -iso.y + overview.yres / 2;

    // Relative to viewport coords.

    const int32_t xx = x + overview.point.x;
    const int32_t yy = y + overview.point.y;

    // Account for rounding error.

    const int32_t we = overview.grid.tile_iso_width - 1;
    const int32_t he = overview.grid.tile_iso_height - 1;

    // Project.

    const int32_t rx = (xx * he + yy * we);
    const int32_t ry = (yy * we - xx * he);
    const int32_t cx = (+2 * rx + we * he * overview.grid.cols) / (2 * we);
    const int32_t cy = (-2 * ry + we * he * overview.grid.rows) / (4 * he);

    const Point cart_raw = { cx, cy };
    const Point cart = {
        1 * cx / he,
        2 * cy / we,
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
    // Reverse project.

    const int32_t hw = overview.grid.tile_iso_width / 2;
    const int32_t hh = overview.grid.tile_iso_height / 2;

    const int32_t xx = (cart.y + cart.x) * hw;
    const int32_t yy = (cart.y - cart.x) * hh;

    // Relative to middle of screen.

    const int32_t mx = xx + overview.xres / 2;
    const int32_t my = yy + overview.yres / 2;

    // Relative to center of tile.

    const int32_t cx = mx - hw * overview.grid.cols;
    const int32_t cy = my - hh;

    const Point iso = {
        cx - overview.point.x,
        cy + overview.point.y,
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
