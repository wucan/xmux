#!/bin/sh
rm -f test 
make test 1>cout.txt 2>cout.txt
./test 1>out_sdt.ts
cat out_sdt.ts out_s1-1.ts >  outout.ts
