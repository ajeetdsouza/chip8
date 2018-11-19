#include "cpu.hpp"

#include <emscripten.h>
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

Cpu *cpu = nullptr;
SDL_Renderer *renderer = nullptr;

extern "C" {
    void load_rom(const std::string& fname) {
        delete cpu;
        cpu = new Cpu(fname);
    }
}

void main_loop() {
    // Check if CPU is initialized
    if (cpu == nullptr)
        return;

    // Get keypresses
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
       if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
           const auto key = keymap.find(event.key.keysym.sym);
           if (key == keymap.end())
               break;

           else if (event.type == SDL_KEYDOWN)
               cpu->keys[key->second] = true;

           else if (event.type == SDL_KEYUP)
               cpu->keys[key->second] = false;
       }
   }

    // Assuming the browser refreshes the screen at 60Hz, this should give us a clock speed of 600Hz.
    for (auto i = 0; i < 10; ++i)
        cpu->cycle();

    if (cpu->redraw) {
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        for (auto y_i = 0; y_i < cpu->frame_buf.size(); ++y_i)
            for (auto x_i = 0; x_i < cpu->frame_buf[y_i].size(); ++x_i)
                if (cpu->frame_buf[y_i][x_i])
                    SDL_RenderDrawPoint(renderer, x_i, y_i);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderPresent(renderer);

        cpu->redraw = false;
    }

    if (cpu->beep) {
       std::cout << "BEEP" << std::endl; // TODO
       cpu->beep = false;
    }
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(255, 255, 0, &window, &renderer);

    SDL_RenderSetLogicalSize(renderer, 64, 32);

    const int fps = -1; // enables optimizations for window.requestAnimationFrame()
    emscripten_set_main_loop(main_loop, fps, 1);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
