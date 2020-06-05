#!/bin/bash

# COUNTS THE NUMBER OF LINES THE C COMPILER MUST
# ULTIMATELY CHEW THROUGH AFTER THE PREPROCESSOR IS INVOKED

TOTAL=0
for FILE in ./src/*.c;
do
    echo "counting $FILE"
    LINES=$(gcc -E $FILE | wc --lines)
    TOTAL=$(( $TOTAL + $LINES ))
done
echo "$TOTAL lines of C to chew through _AFTER_ the C PREPROCESSOR"
