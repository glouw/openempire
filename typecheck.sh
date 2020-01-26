# Compiles the C source using clang and gcc,
# using both C++ and C compilers for extra strength type checking.

set -e

for i in 11 10 01 00
do
    make -C src clean
    time make -C src CLANG=${i:0:1} CPP=${i:1:2}
done
