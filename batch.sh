# Starts the server, and then starts how ever many
# clients specified by $USERS.

BIN=openempires
XRES=1300
YRES=730
USERS=3
CIVS=4
POWER=8
./$BIN --quiet --server --users $USERS --power $POWER &
SERVER_PID=$!
for (( i = 0; i < $USERS; i++ ))
do
    D=20
    X=$(($XRES - $D * i))
    Y=$(($YRES - $D * i))
    CIV=$((i % $CIVS))
    ./$BIN --xres $X --yres $Y --civ $CIV &
    sleep 0.1
done
LAST_PID=$!
wait $LAST_PID
kill $SERVER_PID
