DEV=lo

LATENCY=20ms
VARIANCE=3ms

sudo tc qdisc del dev $DEV root netem
sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss 0.1% 25% duplicate 1% corrupt 0.1%
