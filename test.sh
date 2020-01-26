# Compiles src, sets up network latency on local host,
# starts the server, and starts a number of clients.

make -C src
source netsim.sh
source batch.sh
