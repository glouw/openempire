# Open Empires

Open Empires is an educational multi-player RTS engine which interfaces
with the Age of Empires II Trial assets and strives to be written in 15,000
lines of C99 or less, using only SDL2 as an external dependency.

![](https://user-images.githubusercontent.com/7000616/76479321-9ef9c700-63c8-11ea-9353-a7dafa29ea0a.png)

## Dependencies

    Arch   : pacman -S sdl2 sdl2_net sdl2_ttf
    Ubuntu : apt-get install libsdl2-dev libsdl2-net-dev libsdl2-ttf-dev

## Building

    git clone https://github.com/glouw/openempires
    cd openempires
    make -C src

## Running

Get a copy of Age of Empires II trial version from the internet archive:

    https://archive.org/download/AgeofEmpiresIITheAgeofKings_1020/AoE2demo.zip

Unzip, and run the installation .exe with wine. The game's asset data folder
can be found in the root directory of the game's installation:

    "$HOME/.wine/drive_c/Program Files (x86)/Microsoft Games/Age of Empires II Trial/Data"

*The EULA agreement states that you may share the installer as a whole,
but individual files installed by the installer may not be shared across computers.*

Once installed, the quickest way to get the client server model tested
is to edit GAME_PATH in test.sh to be your game's installation DATA folder,
although if you are on a UNIX like machine this should already be correctly set.
Once done, run:

    ./test.sh

This test script runs netem which purposely emulates a laggy lossy network.
Change ENTROPY within test.sh to to set realistic or development playing conditions.

Otherwise, if you are looking to piece-wise setup the server and client on localhost,
first start the server by specifying the port and the number of users who will be playing:

    ./openempires --server --port 1234 --users 3

Keep note, that 3 users implies 2 human players, and 1 anti-cheat bot
who spectates the game and detects and restores out of syncs.

Second, start these 3 clients (you, on your computer, a friend on their computer,
and the anti-cheat bot on a computer you trust). The anti cheat bot must be started last.
For instance, to start 1 client:

    ./openempires --host localhost --port 1234 --xres 1440 --yres 900 --path "/path/to/data/folder"

The data folder path is the same path previously outline above.

From here on its the game you know and love. Some small tweaks have been added
to ease some of the controls:

    W A S D :
        pans the camera

    Left shift + left click:
        selects multiple of the same unit

    Left alt + Q W E R T ... Z X C V B:
        selects a building to place, or a unit command to execute.

## Structure

C source lives in src/ folder and is compilable by both C++11 and C11
compilers. C source is separated by typedefs, implemented with .[ch]
files. General purpose scripts may live in the top level.

Structs are passed by value such that functions calls modify no more than
one argument at a time. Link time optimization removes most of the heavy
function argument copying.

To simplify the lockstep engine, double and floating points are not used,
and the standard math library is not linked with the final binary.

## Legal

Open Empires was written from scratch with the highest regards of the
original copyright holders.

Open Empires is *NOT* a binary decompilation of Age of Empires II.

Data, art, and sound files from Age of Empires II, for trial and non-trial
assets alike - ARE NOT, AND NEVER WILL BE, SUPPLIED.

Age of Empires II is copyright (©) by Microsoft Corporation and Ensemble Studios.
Open Empires was created under Microsoft's Game Content Usage Rules:

    https://www.xbox.com/en-us/developers/rules

Open Empires uses assets from Age of Empires II, and it is not endorsed by
or affiliated with Microsoft.

## Credits

Thanks to:

* SFTech OpenAge team for their fantastic documentation on the
  inner workings of the SLP and DRS file formats. See:
      https://github.com/SFTtech/openage

* The developers and artists from Ensemble Studios for the many
  gifted childhood memories

* Forgotten Empires for keeping the spirit of Age of Empires II alive

Dedicated to Ondrej whom I met hiking across England as he hiked from
Scotland to Spain.

For the Josh Master. Volare, my friend, wherever you are.
