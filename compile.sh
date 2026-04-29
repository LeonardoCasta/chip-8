#!/bin/bash

gcc chip-8.c ./cpu/cpu.c ./keyboard/keyboard.c -I ./raylib -L ./raylib -lraylib -lm -lpthread -ldl -lrt -lX11