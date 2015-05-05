#!/bin/sh
uname -m
gcc *.c *.cpp -lm -lpthread -lstdc++ -Wall -g -o "public"