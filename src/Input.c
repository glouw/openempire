#include "Input.h"

#include <SDL2/SDL.h>

Input Input_Pump(Input input)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    if(input.key[SDL_SCANCODE_END] || input.key[SDL_SCANCODE_ESCAPE])
        input.done = true;

    const uint32_t buttons = SDL_GetRelativeMouseState(&input.dx, &input.dy);

    input.l = (buttons >> 0) & 0x1;
    input.m = (buttons >> 1) & 0x1;
    input.r = (buttons >> 2) & 0x1;

    input.lu = (input.l == 0 && input.ll == 1);
    input.mu = (input.m == 0 && input.lm == 1);
    input.ru = (input.r == 0 && input.lr == 1);

    input.ld = (input.l == 1 && input.ll == 0);
    input.md = (input.m == 1 && input.lm == 0);
    input.rd = (input.r == 1 && input.lr == 0);

    input.ll = input.l;
    input.lm = input.m;
    input.lr = input.r;

    int32_t x = 0;
    int32_t y = 0;
    SDL_GetMouseState(&x, &y);

    input.point.x = x;
    input.point.y = y;

    if(input.ld) input.ld_point = input.point;
    if(input.rd) input.rd_point = input.point;

    return input;
}

Input Input_Ready(void)
{
    static Input zero;
    Input input = zero;
    input.key = SDL_GetKeyboardState(NULL);
    return Input_Pump(input);
}
