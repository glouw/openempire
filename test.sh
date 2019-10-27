set -e

for i in 11 10 01 00
do
    make -C src clean
    time make -C src SANITIZE=0 CLANG=${i:0:1} CPP=${i:1:2}
done
