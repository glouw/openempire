## Open Empires

Open Empires is an educational Genie Engine implementation capable of interfacing with the Age of Empires II and The Conquerors art assets.

Open Empires is multi-player only and strives to be written in less than 15,000 lines of C99 using only SDL2.

## Legal

Open Empires is a clean room implementation of the Genie Engine and was written from scratch with the highest regards of the original copyright holders.

Open Empires is *NOT* a binary decompilation of Age of Empires II, and its expansion, The Conquerors.

Data, art, and sound files from Age of Empires II - or its expansion, The Conquerors - ARE NOT, AND NEVER WILL BE, SUPPLIED.
To play Open Empires you must legally own the original boxed-set version of Age of Empires II and The Conquerors,
preferably the Gold Edition.

Age of Empires II and The Conquerors are copyright (©) by Microsoft Corporation and Ensemble Studios.
Open Empires was created under Microsoft's ["Game Content Usage Rules"](https://www.xbox.com/en-us/developers/rules)
using assets from Age of Empires II and The Conquerors, and it is not endorsed by or affiliated with Microsoft.

## Dependencies

    Arch   : pacman -S sdl2 sdl2_net sdl2_ttf
    Ubuntu : apt-get install libsdl2-dev libsdl2-net-dev libsdl2-ttf-dev

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src

## Running

To test the client-server model, run:

    ./test.sh

Otherwise, assuming everything is on localhost, first start the server:

    ./openempires --server --port 1234

Second, start the client by specifying your window resolution and installation data folder path:

    ./openempires --host localhost --port 1234 --xres 1440 --yres 900 --path "/path/to/data/folder"

## Status

* [x] Multithreaded software renderer
* [x] Random map generation
* [x] Resources
* [x] 8 player TCP client / server model
* [x] Melee combat
* [x] Unit upgrades
* [x] All ages and building updates
* [ ] Ranged combat
* [ ] Unit and building build times and resource use
* [ ] Sound

## Structure

C source lives in src/ folder and is compilable by both C++11 and C11 compilers.
C source is separated by typedefs, implemented with .[ch] files.
General purpose scripts may live in the top level.

## Credits

Thanks to:
* SFTech OpenAge team for their fantastic documentation on the inner workings of the SLP and DRS file formats (https://github.com/SFTtech/openage)
* The developers and artists from Ensemble Studios for the many gifted childhood memories
* Forgotten Empires for keeping the spirit of Age of Empires II alive

Dedicated to Ondrej whom I met hiking across England as he hiked from Scotland to Spain.

For the Josh Master. Volare, my friend, wherever you are.
