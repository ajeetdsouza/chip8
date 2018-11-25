# chip8

A WebAssembly CHIP8 emulator written in C++17 and SDL2. 

[Demo](https://ajeetdsouza.gitlab.io/chip8/)

## Building

- Before installing, ensure you have the [Emscripten SDK](https://github.com/juj/emsdk.git) installed and sourced.
- Run `./build.sh`.

## Usage

- Build the project.
- Start up an HTTP server: `python3 -m http.server 5000` (this is not necessary if you use Firefox)
- Open `index.html` in a browser and load a game from the dropdown menu.
- Since this is an emulator, the CHIP8 keys have been remapped to `1234`, `qwer`, `asdf` and `zxcv`. This might be an awkward key combination for some games, but it works.
