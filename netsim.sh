# Adds some latency, packet loss, packet loss, and packet corruption to localhost
# to simulate world wide latency effects.

DEV=lo

LATENCY=50ms
VARIANCE=5ms

sudo tc qdisc del dev $DEV root netem
sudo tc qdisc add dev $DEV root netem delay $LATENCY $VARIANCE 25% loss 0.1% 25% duplicate 1% corrupt 0.1%
