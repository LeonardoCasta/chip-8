#!/bin/bash

gcc chip-8.c -I ./raylib -L ./raylib -lraylib -lm -lpthread -ldl -lrt -lX11