#!/bin/bash

build()
{
    CPUS=$(nproc --all)
    T=$(($CPUS + 1))
    make -j$T -C src
}

netsim()
{
    DEV=lo
    LATENCY=25ms
    VARIANCE=5ms

    ENTROPY=2.0% # SET TO 0.1% FOR REALISTIC NETWORKING EMULATION. SET TO 2.0% FOR STRESS TESTING.
    sudo tc qdisc del dev $DEV root netem
    sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss $ENTROPY 25% duplicate $ENTROPY corrupt $ENTROPY
}

batch()
{
    BIN=openempires
    XRES=1300
    YRES=700
    USERS=2
    CIVS=4
    POWER=7
    HOST=localhost
    PORT=1111
    ./$BIN --server --quiet --users $USERS --power $POWER --port $PORT &
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
