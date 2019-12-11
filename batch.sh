# BATCH SIM.
# NUMBER A NUMBER OF USERS TO A LOCALHOST SERVER.
main()
{
    BIN=openempires
    XRES=1000
    YRES=700
    USERS=8

    # START THE LOCALHOST SERVER. MARK DOWN ITS PID.
    ./$BIN --server --users $USERS --quiet &
    SERVER_PID=$!

    # INSTANTIATE A NUMBER OF USER CLIENTS TO CONNECT TO THE LOCALHOST SERVER.
    for (( i = 0; i < $USERS; i++ ))
    do
        # USER CLIENTS WILL HAVE DIFFERENT SCREEN RESOLUTIONS FOR TESTING AND VISIBILITY PURPOSES.
        D=20
        X=$(($XRES - $D * i))
        Y=$(($YRES - $D * i))
        ./$BIN --xres $X --yres $Y &
        sleep 0.1
    done

    # WAIT FOR THE MOST RECENT USER CLIENT PID TO EXIT. KILL THE SERVER PID IMMEDIATELY AFTER.
    LAST_PID=$!
    wait $LAST_PID
    kill $SERVER_PID
}

main
