#include <cstdlib>
#include <cstddef>
#include <algorithm>
#include <cmath>

#include "SDL.h"
#include "player.h"
//#include "map.h"

Player::Player(float pos_x = 2., float pos_y = 2., float ang = 1., float fov = 1., float t = .0, float w = .0) :
    x(pos_x), y(pos_y), a(ang), fov(fov), turn(t), walk(w) {}


void Player::handle_events(GameState& gs, float dt) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym)) gs.quit = true;
        if (SDL_KEYUP == event.type) {
            if ('a' == event.key.keysym.sym || 'd' == event.key.keysym.sym) turn = 0;
            if ('w' == event.key.keysym.sym || 's' == event.key.keysym.sym) walk = 0;
        }
        if (SDL_KEYDOWN == event.type) {
            if ('a' == event.key.keysym.sym) turn = -1;
            if ('d' == event.key.keysym.sym) turn = 1;
            if ('w' == event.key.keysym.sym) walk = 1;
            if ('s' == event.key.keysym.sym) walk = -1;
        }
        SDL_SetRelativeMouseMode(SDL_TRUE);
        if (SDL_MOUSEMOTION == event.type) {
            Sint32 mouse_rel_x = event.motion.xrel;
            float xpos = 0;
            xpos += mouse_rel_x;
 
            a += xpos * dt * 0.02F;
        }
    }

    float nx = x + walk * cos(a) * dt*2;
    float ny = y + walk * sin(a) * dt*2;

    if (static_cast<size_t>(nx) >= 0 && static_cast<size_t>(nx) < static_cast<int>(gs.map.w) && static_cast<size_t>(ny) >= 0 && static_cast<size_t>(ny) < static_cast<size_t>(gs.map.h)) {
        if (gs.map.is_empty(nx, static_cast<size_t>(y))) x = nx;
        if (gs.map.is_empty(static_cast<size_t>(x), ny)) y = ny;
    }
    
    for (size_t i = 0; i < gs.monsters.size(); ++i) { // update the distances from the player to each sprite
        gs.monsters[i].player_dist = std::sqrt(pow(x - gs.monsters[i].x, 2) + pow(y - gs.monsters[i].y, 2));
    }
    std::sort(gs.monsters.begin(), gs.monsters.end()); // sort it from farthest to closest
    
}

float Player::get_x() {
    return x;
}
float Player::get_y() {
    return y;
}
float Player::get_angle() {
    return a;
}
float Player::get_fov() {
    return fov;
}
