#include <cstdlib>
#include <cstddef>
#include "sprite.h"

Sprite::Sprite(float pos_x, float pos_y, short texture_id, float player_dis)
    : x(pos_x), y(pos_y), tex_id(texture_id), player_dist(player_dis) {}

bool Sprite::operator < (const Sprite& s) const {
    return player_dist > s.player_dist;
}