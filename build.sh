#!/usr/bin/env bash

em++ -std=c++17 \
     -I./include/ \
     -s USE_SDL=2 \
     -s EXPORTED_FUNCTIONS='["_main", "_load_rom"]' \
     -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]' \
     -s FORCE_FILESYSTEM=1 \
      --preload-file ./roms/ \
     -O3 \
     ./src/main.cpp ./src/cpu.cpp \
     -o docs/chip8.js
