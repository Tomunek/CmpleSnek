#!/bin/bash
set -e
gcc -Wall -Werror -Wpedantic snek.c -o snek
clear
./snek