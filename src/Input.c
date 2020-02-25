#include "Input.h"

#include <SDL2/SDL.h>

static bool ExitRequested(const Input input, const SDL_Event event)
{
    return input.key[SDL_SCANCODE_END]
        || input.key[SDL_SCANCODE_ESCAPE]
        || event.type == SDL_QUIT;
}

Input Input_Pump(Input input)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    if(ExitRequested(input, event))
        input.done = true;
    const uint32_t buttons = SDL_GetRelativeMouseState(NULL, NULL);
    input.l = (buttons >> 0) & 0x1;
    input.r = (buttons >> 2) & 0x1;
    input.lu = (input.l == 0 && input.ll == 1);
    input.ru = (input.r == 0 && input.lr == 1);
    input.ld = (input.l == 1 && input.ll == 0);
    input.rd = (input.r == 1 && input.lr == 0);
    input.ll = input.l;
    input.lr = input.r;
    int32_t x = 0;
    int32_t y = 0;
    SDL_GetMouseState(&x, &y);
    input.cursor.x = x;
    input.cursor.y = y;
    return input;
}

Input Input_Ready(void)
{
    static Input zero;
    Input input = zero;
    input.key = SDL_GetKeyboardState(NULL);
    return Input_Pump(input);
}
