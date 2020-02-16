#pragma once

#include <cstdlib>

class Sprite {
public:
    float x, y;
    size_t tex_id;
    float player_dist;

    bool operator < (const Sprite& s) const;

    Sprite(float pos_x, float pos_y, size_t texture_id, float player_dis);
};

