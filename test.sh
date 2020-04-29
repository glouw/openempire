#!/bin/bash

# 1. BUILDS SOURCE.
# 2. ENABLES A LOSSY LOCAL HOST NETWORK.
# 3. STARTS A SERVER, AND STARTS SEVERAL CLIENTS.

GAME_PATH="$HOME/.wine/drive_c/Program Files (x86)/Microsoft Games/Age of Empires II Trial/Data/";
LATENCY=25ms
VARIANCE=1ms
ENTROPY=0.1% # REALISM = 0.1%. STRESS TEST = 1.1%.
XRES=640
YRES=480
USERS=3
MAP_SIZE=64
HOST=localhost
PORT=1111

build()
{
    CPUS=$(nproc --all)
    T=$(($CPUS + 1))
    make -C src
}

netsim()
{
    DEV=lo
    ulimit -c 0
    sudo tc qdisc del dev $DEV root netem
    sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss $ENTROPY
}

server()
{
    ./$BIN --server --quiet --users $USERS --map_size $MAP_SIZE --port $PORT
}

client()
{
    ./$BIN --xres $1 --yres $2 --host $HOST --port $PORT --path "$GAME_PATH" &
}

batch()
{
    BIN=openempires
    server &
    for (( i = 0; i < $(($USERS - 1)); i++ ))
    do
        D=20
        X=$(($XRES - $D * i))
        Y=$(($YRES - $D * i))
        client $X $Y &
        sleep 0.2
    done
    # SPECTATOR MUST CONNECT LAST, SO ENSURE WITH A SLEEP THEY COME LAST.
    sleep 5
    client 400 300
}

build
netsim
batch
