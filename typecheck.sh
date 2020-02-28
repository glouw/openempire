#!/bin/bash

# COMPILES SOURCE AS C AND C++ USING BOTH CLANG AND GCC
# TO ENSURE TYPE SAFETY.

set -e

for i in 11 10 01 00
do
    make -C src clean
    time make -C src CLANG=${i:0:1} CPP=${i:1:2}
done
