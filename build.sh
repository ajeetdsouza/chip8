#!/usr/bin/env bash

em++ -std=c++17 \
     -I./include/ \
     -s "EXPORTED_FUNCTIONS=['_main', '_load_rom']" \
     -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" \
     -s USE_SDL=2 \
     -s WASM=1 \
     --closure 1 \
     --preload-file ./roms/ \
     -O3 \
     ./src/main.cpp ./src/cpu.cpp ./src/cpu_opcodes.cpp \
     -o ./site/chip8.js
