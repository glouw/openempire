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

    [x] Multi-threaded assest loading
    [x] Multi-threaded terrain software rendering and blending
    [x] Multi-threaded macro level pathfinding (breadth first search)
    [x] Multi-threaded micro level pathfinding (boids)
    [x] Multi-threaded unit software rendering
    [x] Fast compile times
    [ ] Melee combat
    [ ] Ranged combat
    [ ] Multiplayer networking

## Development Blog

http://glouw.com/2019/04/10/Reverse-Engineering-Age-of-Empires-2-Part-0.html

## Credits

Thanks to the SFTech OpenAge team for their fantastic reverse engineering documentation.

https://github.com/SFTtech/openage
