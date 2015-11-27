#!/bin/sh
uname -m 
gcc src/*.c src/*.cpp -I/usr/include/postgresql/ -lm -lpthread -lstdc++ -lpq -Wall -g3 -o "public"