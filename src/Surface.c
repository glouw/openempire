#include "Surface.h"

void Surface_PutPixel(SDL_Surface* const surface, const int32_t x, const int32_t y, const uint32_t color)
{
    uint32_t* const pixels = (uint32_t*) surface->pixels;
    pixels[x + y * surface->w] = color;
}

uint32_t Surface_GetPixel(SDL_Surface* const surface, const int32_t x, const int32_t y)
{
    uint32_t* const pixels = (uint32_t*) surface->pixels;
    return pixels[x + y * surface->w];
}

void Surface_Clear(SDL_Surface* const surface, const uint32_t color)
{
    for(int32_t x = 0; x < surface->w; x++)
    for(int32_t y = 0; y < surface->h; y++)
        Surface_PutPixel(surface, x, y, color);
}

SDL_Surface* Surface_New(const int32_t width, const int32_t height)
{
    SDL_Surface* const surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, SURFACE_BIT_DEPTH, SURFACE_PIXEL_FORMAT);
    Surface_Clear(surface, SURFACE_COLOR_KEY);
    return surface;
}
