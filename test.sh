#!/bin/bash

# BUILDS SOURCE, ENABLES A LOSSY LOCAL HOST NETWORK, STARTS A SERVER, AND STARTS SEVERAL CLIENTS

GAME_PATH="/home/gl/.wine/drive_c/Program Files (x86)/Microsoft Games/Age of Empires II Trial ARCHIVE ORG/Data/";
LATENCY=50ms
VARIANCE=5ms
ENTROPY=0.1% # REALISM = 0.1%. STRESS TEST = 2.1%.
XRES=1200
YRES=700
USERS=3
MAP_SIZE=64
HOST=localhost
PORT=1111

build()
{
    CPUS=$(nproc --all)
    T=$(($CPUS + 1))
    make -j$T -C src
}

netsim()
{
    DEV=lo
    sudo tc qdisc del dev $DEV root netem
    sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss $ENTROPY 25% duplicate $ENTROPY corrupt $ENTROPY
}

batch()
{
    BIN=openempires
    ./$BIN --server --users $USERS --map_size $MAP_SIZE --port $PORT &
    for (( i = 0; i < $(($USERS - 1)); i++ ))
    do
        D=20
        X=$(($XRES - $D * i))
        Y=$(($YRES - $D * i))
        ./$BIN --xres $X --yres $Y --host $HOST --port $PORT --path "$GAME_PATH" &
    done
    # SPECTATOR MUST CONNECT LAST, SO ENSURE WITH A SLEEP THEY COME LAST.
    sleep 2
    ./$BIN --xres 400 --yres 300 --host $HOST --port $PORT --path "$GAME_PATH"
}

build
netsim
batch
