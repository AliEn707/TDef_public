#!/bin/sh
uname -m 
gcc src/*.c -I/usr/include/postgresql/ -lm -lpthread -lstdc++ -lpq -Wall -g3 -o "public"