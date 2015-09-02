#!/bin/sh
uname -m 
gcc src/*.c src/*.cpp -I/usr/include/postgresql/ -march=native -lm -lpthread -lstdc++ -lpq -Wall -O3 -ffast-math -fsigned-char -fgnu89-inline -o "public"