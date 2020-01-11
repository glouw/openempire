DEV=lo

sudo tc qdisc del dev $DEV root netem
sudo tc qdisc add dev $DEV root netem delay 50ms 10ms 25% loss 0.1% 25% duplicate 1% corrupt 0.1%
