#pragma once

#include <cstdlib>

class Sprite {
public:
    float x, y;
    short tex_id;
    float player_dist;

    bool operator < (const Sprite& s) const;

    Sprite(float pos_x, float pos_y, short texture_id, float player_dis);
};

