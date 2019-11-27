![](art/logo.png)

This is a custom game engine for Age of Empires II written in C99 and SDL2 built for fast network play.

Data, art, and sound files are not supplied - to play Open Empires you must either own the original boxed-set
of Age of Empires II and The Conquerors, or purchase a copy of Age II HD on Steam.

## Dependencies

    sdl2
    sdl2_ttf
    sdl2_net

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src

## Running

First, start the server.

    ./openempires --server

Second, start the client. Point the path flag to the Age of Empires 2 data installation folder on Steam.

    ./openempires --path "/home/gl/.steam/steam/steamapps/common/Age2HD/Directory/data/"

If on Linux, install Age 2 using Steam Proton.

## Current Development Progress

    [x] Multi-threaded assest loading
    [x] Multi-threaded terrain software rendering and blending
    [x] Multi-threaded macro level pathfinding (breadth first search)
    [x] Multi-threaded macro level pathfinding (Gready best search)
    [x] Multi-threaded micro level pathfinding (boids)
    [x] Multi-threaded unit software rendering
    [x] Unit state switching
    [x] Melee combat
    [x] Multiplayer networking (currently one way TCP)
    [x] Fast compile times with link time optimization (full build: 8.346 seconds on a Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz)
    [ ] Ranged combat

## Development Blog (on hiatus)

http://glouw.com/2019/04/10/Reverse-Engineering-Age-of-Empires-2-Part-0.html

## Credits

Thanks to:
* SFTech OpenAge team for their fantastic reverse engineering documentation (https://github.com/SFTtech/openage)
* Ensemble Studios for many gifted childhood memories
* Forgotten Empires for keeping the spirit of Age of Empires II alive
