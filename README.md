![](art/logo.png)

This is a custom game engine for Age of Empires II written in pure C99 and SDL2 built for fast network play.

The inspiration for this engine draws from combining core Age II mechanics and Rome Total War style battles.

Data, art, and sound files are not supplied - to play Open Empires you must either own the original boxed-set
of Age of Empires II and The Conquerors, or purchase a copy of Age II HD on Steam.

## Dependencies

    sdl2
    sdl2_ttf

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src

## Running

Assuming you were able to install Age of Empires II HD with steam on Linux (enable Proton under Beta options in settings),
simply run the compiled executable with the path flag pointing to the data folder of your game's installation:

    ./openempires --path "/home/gl/.steam/steam/steamapps/common/Age2HD/Directory/data/"

## Current Development Progress

    [x] Multi-threaded assest loading
    [x] Multi-threaded terrain software rendering and blending
    [x] Multi-threaded macro level pathfinding (breadth first search)
    [x] Multi-threaded micro level pathfinding (boids)
    [x] Multi-threaded unit software rendering
    [x] Unit state switching
    [x] Melee combat
    [ ] Ranged combat
    [ ] Multiplayer networking
    [x] Fast compile times with link time optimization (full build: 5.157 seconds on a Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz)

## Development Blog

http://glouw.com/2019/04/10/Reverse-Engineering-Age-of-Empires-2-Part-0.html

## Credits

Thanks to the SFTech OpenAge team for their fantastic reverse engineering documentation.

https://github.com/SFTtech/openage

Thanks to Ensemble Studios for many hours of gifted childhood memories.

Thanks to Forgotten Empires for keeping the spirit of Age of Empires alive.
