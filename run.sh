# !/bin/sh

./build
./bin/tlang example/input002.tl
cat out.asm
fasm2 out.asm
ld out.o -o out -dynamic-linker /usr/lib64/ld-linux-x86-64.so.2 -lc
./out
echo $?
