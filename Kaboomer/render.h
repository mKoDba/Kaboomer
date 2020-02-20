#pragma once

#include <vector>
#include <cstdint>

#include "map.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"

class Player;

struct GameState {
    Map map;
    std::vector<Sprite> monsters;
    Texture tex_walls;
    Texture tex_monst;
    Texture tex_weapon;
    bool quit = false;
};

void render(FrameBuffer& fb, const GameState& gs, Player& player);

