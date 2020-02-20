#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <cmath>
#include "SDL.h"

#include "utils.h"
#include "render.h"
#include "player.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    constexpr unsigned fps = 60;

    FrameBuffer fb{};
    Player player{ 3.456F, 2.345F, 1.523F, static_cast<float>((M_PI / 3.)), 0, 0 };      // player
    GameState gs{ Map(),                                        // game map
                  { {3.523, 3.812, 2, 0},                       // monsters lists
                    {1.834, 8.765, 0, 0},
                    {5.323, 5.365, 1, 0},
                    {14.32, 13.36, 3, 0},
                    {4.123, 10.76, 1, 0} },
                  Texture("./walls.bmp", SDL_PIXELFORMAT_ABGR8888),  // textures for the walls
                  Texture("./monsters.bmp", SDL_PIXELFORMAT_ABGR8888),  // textures for the monsters
                  Texture("./AngledPistol.bmp", SDL_PIXELFORMAT_ABGR8888) };     // texture for the weapon
    if (!gs.tex_walls.count || !gs.tex_monst.count) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;

    window = SDL_CreateWindow("Kaboomer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    if (window == NULL) {
        std::cerr << "Failed to create sdl_window: " << SDL_GetError() << std::endl;
        return -1;
    }
    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer == NULL) {
        std::cerr << "Failed to create sdl_renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture* framebuffer_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
    if (!framebuffer_texture) {
        std::cerr << "Failed to create framebuffer texture : " << SDL_GetError() << std::endl;
        return -1;
    }

    Uint32 last_time = SDL_GetTicks();

    // GAME LOOP
    while (!gs.quit) {

        float dt = (SDL_GetTicks() - last_time) / 1000.0F;     // in seconds
        last_time = SDL_GetTicks();
        if (dt == 0) dt = 1;

        player.handle_events(gs, dt);

        render(fb, gs, player); // render the scene to the framebuffer
        
        // copy the framebuffer contents to the screen
        SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void*>(fb.img.data()), fb.w * 4);
        SDL_RenderClear(sdl_renderer);
        SDL_RenderCopy(sdl_renderer, framebuffer_texture, NULL, NULL);
        SDL_RenderPresent(sdl_renderer);
        

        // stable framerate
        if ((1000.0 / fps) > (SDL_GetTicks() - last_time)) {
            SDL_Delay((1000.0 / fps) - (SDL_GetTicks() - last_time));
        }

    }

    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}