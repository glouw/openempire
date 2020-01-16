## Open Empires

This is a custom game engine for Age of Empires II written in C99 and SDL2 built for fast network play.

Data, art, and sound files are not supplied - to play Open Empires you must own the original boxed-set version of Age of Empires II and The Conquerors.

![](art/screenshot.png)

## Dependencies

    sdl2
    sdl2_ttf
    sdl2_net

### Ubuntu 18.04
    sudo apt-get install libsdl2-dev
    sudo apt-get install libsdl2-net-dev
    sudo apt-get install libsdl2-ttf-dev

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src

## Running

First, start the server.

    ./openempires --server

Second, start the client. Point the path flag to the installation's data folder.

    ./openempires --path "/path/to/data/folder"

## Development Blog (on hiatus)

http://glouw.com/2019/04/10/Reverse-Engineering-Age-of-Empires-2-Part-0.html

## Credits

Thanks to:
* SFTech OpenAge team for their fantastic reverse engineering documentation (https://github.com/SFTtech/openage)
* Ensemble Studios for many gifted childhood memories
* Forgotten Empires for keeping the spirit of Age of Empires II alive

Dedicated to Ondrej who I met walking across England as he hiked from Scotland to Spain.
For the Josh Master. Volare, my friend.
