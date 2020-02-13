#!/bin/bash

# BUILDS SOURCE WITH HOW EVER MANY CPUS ARE PRESENT.
build()
{
    CPUS=$(nproc --all)
    T=$(($CPUS + 1))
    make -j$T -C src
}

# SETS PACKET LOSS AND LATENCY ON LOCALHOST.
netsim()
{
    DEV=lo
    LATENCY=25ms
    VARIANCE=5ms
    DUPLICATE=1%
    # XXX. PACKET CORRUPTION AND PACKET LOSS WILL TRIGGER TCP TO RESEND ITS COMMAND AND MISS BUFFER DEADLINE.
    CORRUPT=1.5%
    LOSS=1.5%
    sudo tc qdisc del dev $DEV root netem
    sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss $LOSS 25% duplicate $DUPLICATE corrupt $CORRUPT
}

# SPAWNS A SERVER AND SOME NUMBER OF CLIENTS ON LOCALHOST.
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
    ./$BIN --server --users $USERS --power $POWER --port $PORT &
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
