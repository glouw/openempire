#!/bin/bash

# BUILDS SOURCE, ENABLES A LOSSY LOCAL HOST NETWORK, STARTS A SERVER,
# AND STARTS SEVERAL CLIENTS OF DIFFERENT CIVILIZATION.

LATENCY=25ms
VARIANCE=5ms
ENTROPY=0.3% # REALISM = 0.1%. STRESS TEST = 2.1%.
XRES=1200
YRES=600
USERS=1
CIVS=4
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
    ./$BIN --server --quiet --users $USERS --map_size $MAP_SIZE --port $PORT &
    SERVER_PID=$!
    for (( i = 0; i < $USERS; i++ ))
    do
        D=20
        X=$(($XRES - $D * i))
        Y=$(($YRES - $D * i))
        CIV=$((i % $CIVS))
        ./$BIN --xres $X --yres $Y --civ $CIV --host $HOST --port $PORT &
    done
    LAST_PID=$!
    wait $LAST_PID
    kill $SERVER_PID
}

build
netsim
batch
