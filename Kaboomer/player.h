#pragma once

#include "render.h"

class Player {
public:
    Player(float pos_x, float pos_y, float ang, float fov, float turn, float walk);
    void handle_events(GameState& gs, float dt);
    float get_x();
    float get_y();
    float get_angle();
    float get_fov();

private:
    float x, y; // position
    float a;    // view direction
    float fov;  // field of view
    float turn, walk;

};