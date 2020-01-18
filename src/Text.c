#include "Text.h"

#include "Surface.h"

#include "Util.h"

Text Text_Build(const char* const path, const int32_t size, const uint32_t color)
{
    if(!TTF_WasInit())
        TTF_Init();
    static Text zero;
    Text text = zero;
    text.font = TTF_OpenFont(path, size);
    if(text.font == NULL)
        Util_Bomb("Could not open %s\n", path);
    text.color.r = (color >> SURFACE_R_SHIFT) & 0xFF;
    text.color.g = (color >> SURFACE_G_SHIFT) & 0xFF;
    text.color.b = (color >> SURFACE_B_SHIFT) & 0xFF;
    return text;
}

static SDL_Texture* GetTexture(const Text text, SDL_Renderer* const renderer, const int32_t alpha, const char* string)
{
    SDL_Surface* const surface = TTF_RenderText_Blended(text.font, string, text.color);
    SDL_Texture* const texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureAlphaMod(texture, alpha < 0 ? 0 : alpha);
    SDL_FreeSurface(surface);
    return texture;
}

static SDL_Rect CalculateSize(const Text text, const char* const string)
{
    int32_t w = 0;
    int32_t h = 0;
    TTF_SizeText(text.font, string, &w, &h);
    const SDL_Rect size = { 0, 0, w, h };
    return size;
}

static void Render(const Text text, SDL_Renderer* const renderer, const SDL_Rect target, const char* const string, const int32_t alpha)
{
    SDL_Texture* const texture = GetTexture(text, renderer, alpha, string);
    SDL_RenderCopy(renderer, texture, NULL, &target);
    SDL_DestroyTexture(texture);
}

static void Write(const Text text, SDL_Renderer* const renderer, const Point point, const Position position, const int32_t alpha, const int32_t line, const char* const string)
{
    const SDL_Rect sz = CalculateSize(text, string);
    SDL_Rect to = { 0, 0, sz.w, sz.h };
    switch(position)
    {
    case POSITION_TOP_LEFT:
        to.x = point.x;
        to.y = point.y + line * sz.h;
        break;

    case POSITION_TOP_RITE:
        to.x = point.x - sz.w;
        to.y = point.y + line * sz.h;
        break;

    case POSITION_BOT_LEFT:
        to.x = point.x;
        to.y = point.y - sz.h + line * sz.h;
        break;

    case POSITION_BOT_RITE:
        to.x = point.x - sz.w;
        to.y = point.y - sz.h + line * sz.h;
        break;

    case POSITION_MIDDLE:
        to.x = point.x - sz.w / 2;
        to.y = point.y - sz.h / 2 + line * sz.h;
        break;
    }
    Render(text, renderer, to, string, alpha);
}

int32_t Text_Puts(const Text text, SDL_Renderer* const renderer, const Point point, const Position position, const int32_t alpha, const int32_t line, const char* const string)
{
    char* const copy = Util_StringDup(string);
    const char* const delimeter = "\n";
    int32_t newlines = 0;
    for(char* token = strtok(copy, delimeter); token; token = strtok(NULL, delimeter))
        Write(text, renderer, point, position, alpha, line + newlines++, token);
    free(copy);
    return newlines;
}

int32_t Text_Printf(const Text text, SDL_Renderer* const renderer, const Point point, const Position position, const int32_t alpha, const int32_t line, const char* const format, ...)
{
    va_list args;
    va_start(args, format);
    const int32_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    va_start(args, format);
    char* const string = UTIL_ALLOC(char, len + 1);
    vsprintf(string, format, args);
    va_end(args);
    const int32_t newlines = Text_Puts(text, renderer, point, position, alpha, line, string);
    free(string);
    return newlines;
}

void Text_Free(const Text text)
{
    TTF_CloseFont(text.font);
}
