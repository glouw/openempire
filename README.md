# Open Empires

Open Empires is an educational multi-player RTS engine which interfaces with the Age of Empires II Trial assets.

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

To play, first start the server by specifying the port and the number of users
who will be playing (the data folder path is above):

    ./openempires --server --port 1234 --users 3 --path "/path/to/data/folder"

Keep note, that three users implies two human players and one anti-cheat bot
who spectates the game and detects and restores out of syncs.

Second, start these three clients (you, on your computer, a friend on their computer,
and the anti-cheat bot on a computer you trust). The anti cheat bot must be started last.
For instance, to start one client:

    ./openempires --host localhost --port 1234 --xres 1440 --yres 900 --path "/path/to/data/folder"

## Controls

Some small tweaks have been added to ease some of the controls:

    W A S D :
        pans the camera

    Left shift + left click:
        selects multiple of the same unit

    Left alt + Q W E R T ... Z X C V B:
        selects a building to place, or a unit command to execute.

## Legal

Open Empires was written from scratch with the highest regards of the
original copyright holders.

Open Empires is *NOT* a binary decompilation of Age of Empires II.

Data, art, and sound files from the Age of Empires II Trial are not supplied.

Age of Empires II is copyright (©) by Microsoft Corporation, Ensemble Studios, and SkyBox Labs.
Open Empires was created under Microsoft's Game Content Usage Rules:

    https://www.xbox.com/en-us/developers/rules

Open Empires uses assets from the Age of Empires II Trial, and it is not endorsed by
or affiliated with Microsoft, Ensemble Studios, or SkyBox Labs.

## Credits

Thanks to:

* SFTech OpenAge team for their fantastic documentation on the
  inner workings of the SLP and DRS file formats. See:
      https://github.com/SFTtech/openage

* The developers and artists from Ensemble Studios for the many
  gifted childhood memories

* Forgotten Empires for keeping the spirit of Age of Empires II alive

Dedicated to Ondrej whom I met hiking across England as he hiked from
Scotland to Spain, and to TheJoshMaster22; volare, my friend, wherever you are.
