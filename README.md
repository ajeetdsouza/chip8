# chip8

[![Build Status](https://travis-ci.org/ajeetdsouza/chip8.svg?branch=master)](https://travis-ci.org/ajeetdsouza/chip8)

A WebAssembly CHIP8 emulator written in C++17 and SDL2.

[Demo](https://ajeetdsouza.github.io/chip8/)

## Building

- Before installing, ensure you have the [Emscripten SDK](https://github.com/juj/emsdk.git) installed and sourced.
- Run `./build.sh`.

## Usage

- Build the project.
- Start up an HTTP server from the `site` directory: `python3 -m http.server 5000`
- Open `index.html` in a browser and load a game from the dropdown menu.
- CHIP8 keys have been remapped to `1234`, `qwer`, `asdf` and `zxcv`.
