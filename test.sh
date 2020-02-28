#!/bin/bash

# BUILDS SOURCE, ENABLES A LOSSY LOCAL HOST NETWORK, STARTS A SERVER,
# AND STARTS SEVERAL CLIENTS OF DIFFERENT CIVILIZATION.

LATENCY=25ms
VARIANCE=5ms
ENTROPY=2.1% # REALISM = 0.1%. STRESS TEST = 2.1%.
XRES=1300
YRES=700
USERS=2
CIVS=4
POWER=7
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
    ./$BIN --server --quiet --users $USERS --power $POWER --port $PORT &
    SERVER_PID=$!
    for (( i = 0; i < $USERS; i++ ))
    do
        sleep 3
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
