## Open Empires

This is a custom game engine for Age of Empires II written in less than 15,000 lines of C99 using only SDL2.

Open Empires is multiplayer only, and borrows elements from StarCraft II and Age of Empires III.

Data, art, and sound files are not supplied - to play Open Empires you must own the original boxed-set version of Age of Empires II and The Conquerors.

Assets from the HD edition (not the definitive edition) will soon be supported.

![](art/screenshot.png)

## Dependency Installation

Assuming root, install the following SDL2 libraries:

### Arch Linux

    pacman -S sdl2 sdl2_ttf sdl2_net

### Ubuntu

    apt-get install libsdl2-dev libsdl2-net-dev libsdl2-ttf-dev

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

Dedicated to Ondrej whom I met hiking across England as he hiked from Scotland to Spain.

For the Josh Master. Volare, my friend, wherever you are.
