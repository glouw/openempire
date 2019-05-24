![](art/logo.png)

This is a reverse engineering of Age of Empires II intended for fast network play.

## Dependencies

    sdl2
    sdl2_ttf

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src
    ./openempires --path "/home/gl/.steam/steam/steamapps/common/Age2HD/Directory/data/"

## Current Progress

    [x] Assest loading
    [x] Multithreaded CPU rendering
    [ ] Pathfinding
    [ ] Multiplayer networking

## Development Blog

http://glouw.com/2019/04/10/Reverse-Engineering-Age-of-Empires-2-Part-0.html

## Credits

Thanks to the SFTech OpenAge team for their fantastic reverse engineering documentation.

https://github.com/SFTtech/openage
