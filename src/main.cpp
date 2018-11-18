#include "../include/cpu.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <unordered_map>

const std::unordered_map<SDL_Keycode, int> keymap = {
        {SDLK_x, 0x0},
        {SDLK_1, 0x1},
        {SDLK_2, 0x2},
        {SDLK_3, 0x3},
        {SDLK_q, 0x4},
        {SDLK_w, 0x5},
        {SDLK_e, 0x6},
        {SDLK_a, 0x7},
        {SDLK_s, 0x8},
        {SDLK_d, 0x9},
        {SDLK_z, 0xA},
        {SDLK_c, 0xB},
        {SDLK_4, 0xC},
        {SDLK_r, 0xD},
        {SDLK_f, 0xE},
        {SDLK_v, 0xF}
};

int main(int argc, char *argv[]) {
    Cpu cpu;

    if (argc != 2) {
        std::cout << "Usage: ./chip8 <ROM file>" << std::endl;
	return 1;
    }
    cpu.load(argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }

    auto window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              640, 320, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    auto renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, 64, 32);

    SDL_Event event;

    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                const auto key = keymap.find(event.key.keysym.sym);
                if (key == keymap.end())
                    break;

                else if (event.type == SDL_KEYDOWN)
                    cpu.press_key(key->second);

                else if (event.type == SDL_KEYUP)
                    cpu.release_key(key->second);
            }
        }

        cpu.cycle();

        if (cpu.redraw) {
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

            for (auto y_i = 0; y_i < cpu.frame_buf.size(); ++y_i)
                for (auto x_i = 0; x_i < cpu.frame_buf[y_i].size(); ++x_i)
                    if (cpu.frame_buf[y_i][x_i])
                        SDL_RenderDrawPoint(renderer, x_i, y_i);

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderPresent(renderer);

            cpu.redraw = false;
        }

        if (cpu.beep) {
            std::cout << "beep" << std::endl;

            cpu.beep = false;
        }
    }
}
