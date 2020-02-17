#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "map.h"
#include "utils.h"
#include "player.h"
#include "framebuffer.h"
#include "textures.h"
#include "sprite.h"
#include "render.h"


int wall_x_texcoord(const double wallX, const Texture& tex_walls) {
    int x_texcoord = static_cast<int>(wallX * tex_walls.size);

    if (x_texcoord < 0) x_texcoord += static_cast<int>(tex_walls.size); // do not forget x_texcoord can be negative, fix that
    assert(x_texcoord >= 0 && x_texcoord < static_cast<int>(tex_walls.size));
    return x_texcoord;
}


void draw_map(FrameBuffer& fb, const std::vector<Sprite>& sprites, const Texture& tex_walls, const Map& map, const size_t cell_w, const size_t cell_h) {
    for (size_t j = 0; j < map.h; ++j) {  // draw the map itself
        for (size_t i = 0; i < map.w; ++i) {
            if (map.is_empty(i, j)) continue; // skip empty spaces
            size_t rect_x = i * cell_w;
            size_t rect_y = j * cell_h;
            size_t texid = map.get(i, j);
            assert(texid < tex_walls.count);
            fb.draw_rectangle(rect_x, rect_y, cell_w, cell_h, tex_walls.get(0, 0, texid)); // the color is taken from the upper left pixel of the texture #texid
        }
    }
    for (size_t i = 0; i < sprites.size(); ++i) { // show the monsters
        fb.draw_rectangle(static_cast<size_t>(sprites[i].x * cell_w - 3), static_cast<size_t>(sprites[i].y * cell_h - 3), 6, 6, pack_color(255, 0, 0));
    }
}


void draw_sprite(FrameBuffer& fb, const Sprite& sprite, const std::vector<float>& depth_buffer, Player& player, const Texture& tex_sprites) {
    // absolute direction from the player to the sprite (in radians)
    float sprite_dir = atan2(sprite.y - player.get_y(), sprite.x - player.get_x());
    while (sprite_dir - player.get_angle() > static_cast<float>(M_PI)) sprite_dir -= 2 * static_cast<float>(M_PI); // remove unncesessary periods from the relative direction
    while (sprite_dir - player.get_angle() < static_cast<float>(-M_PI)) sprite_dir += 2 * static_cast<float>(M_PI);

    size_t sprite_screen_size = std::min(2000, static_cast<int>(fb.h / sprite.player_dist)); // screen sprite size
    int h_offset = static_cast<int>((sprite_dir - player.get_angle()) / player.get_fov() * (fb.w / 2) + (fb.w / 2) / 2 - tex_sprites.size / 2); // do not forget the 3D view takes only a half of the framebuffer
    int v_offset = static_cast<int>(fb.h / 2 - sprite_screen_size / 2);

    for (size_t i = 0; i < sprite_screen_size; ++i) {
        if (h_offset + int(i) < 0 || h_offset + i >= fb.w / 2) continue;
        if (depth_buffer[h_offset + i] < sprite.player_dist) continue; // this sprite column is occluded
        for (size_t j = 0; j < sprite_screen_size; ++j) {
            if (v_offset + int(j) < 0 || v_offset + j >= fb.h) continue;
            uint32_t color = tex_sprites.get(i * tex_sprites.size / sprite_screen_size, j * tex_sprites.size / sprite_screen_size, sprite.tex_id);
            uint8_t r, g, b, a;
            unpack_color(color, r, g, b, a);
            if (a > 128)
                fb.set_pixel(fb.w / 2 + h_offset + i, v_offset + j, color);
        }
    }
}


void render(FrameBuffer& fb, const GameState& gs, Player& player) {
    const Map& map = gs.map;
    const std::vector<Sprite>& sprites = gs.monsters;
    const Texture& tex_walls = gs.tex_walls;
    const Texture& tex_monst = gs.tex_monst;

    fb.clear(pack_color(255, 255, 255)); // clear the screen

    const size_t cell_w = fb.w / (map.w * 2); // size of one map cell on the screen
    const size_t cell_h = fb.h / map.h;

   
    // WALL DRAWING
    std::vector<float> depth_buffer(fb.w / 2, 1e3);
    for (size_t i = 0; i < fb.w / 2; ++i) { // draw the visibility cone AND the "3D" view
        float angle = player.get_angle() - player.get_fov() / 2 + player.get_fov() * i / float(fb.w / 2);

        float rayDirX = cos(angle);
        float rayDirY = sin(angle);
        float distance = 0;
        int stepX = 0, stepY = 0;
        // initial distance to first next x,y square border
        float sideDistX, sideDistY;
        // square (x,y) in which player is
        size_t mapX = int(player.get_x());
        size_t mapY = int(player.get_y());
        bool side;
        size_t texid;

        bool hit = false;

        // initial next square calculation
        if (rayDirX > 0) {
            stepX = 1;
            sideDistX = (mapX + static_cast<float>(1) - player.get_x()) / rayDirX;
        }
        else {
            stepX = -1;
            sideDistX = (player.get_x() - mapX) / static_cast<float>(cos(angle - M_PI));
        }
        if (rayDirY > 0) {
            stepY = 1;
            sideDistY = (mapY + static_cast<float>(1) - player.get_y()) / rayDirY;
        }
        else {
            stepY = -1;
            sideDistY = (player.get_y() - mapY) / static_cast<float>(sin(angle - M_PI));
        }
        //using DDA (Digital Differential Analysis) algorithm
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += std::abs(1 / rayDirX);
                mapX += stepX;
                side = false;
            }
            else {
                sideDistY += std::abs(1 / rayDirY);
                mapY += stepY;
                side = true;
            }

            if (!map.is_empty(mapX, mapY)) {
                hit = true;
                texid = map.get(mapX, mapY);;
            }
        }

        if (side) {   // if horizontal wall was hit
            distance = (mapY - player.get_y() + (1 - stepY) / 2) / rayDirY;
        }
        else {       // vertical wall hit
            distance = (mapX - player.get_x() + (1 - stepX) / 2) / rayDirX;

        }
        //Calculate height of line to draw on screen
        int lineHeight = static_cast<int>((fb.h / (distance * cos(angle - player.get_angle()))));
        depth_buffer[i] = distance * cos(angle - player.get_angle());

        //calculate value of wallX
        double wallX; //where exactly the wall was hit
        if (side == 0) {
            wallX = player.get_y() + distance * rayDirY;
        }
        else {
            wallX = player.get_x() + distance * rayDirX;
        }
        wallX -= floor((wallX));  // get only decimal part

        int x_texcoord = wall_x_texcoord(wallX, tex_walls);
        std::vector<uint32_t> column = tex_walls.get_scaled_column(texid, x_texcoord, lineHeight);
        size_t pix_x = i + fb.w / 2; // we are drawing at the right half of the screen, thus +fb.w/2
        for (size_t j = 0; j < lineHeight; ++j) { // copy the texture column to the framebuffer
            size_t pix_y = j + fb.h / 2 - lineHeight / 2;
            if (pix_y >= 0 && pix_y < (int)fb.h) {
                fb.set_pixel(pix_x, pix_y, column[j]);
            }
        }

        /* floor and ceiling */
        double floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

        //4 different wall directions possible
        if (side == 0 && rayDirX > 0)
        {
            floorXWall = mapX;
            floorYWall = mapY + wallX;
        }
        else if (side == 0 && rayDirX < 0)
        {
            floorXWall = mapX + 1.0;
            floorYWall = mapY + wallX;
        }
        else if (side == 1 && rayDirY > 0)
        {
            floorXWall = mapX + wallX;
            floorYWall = mapY;
        }
        else
        {
            floorXWall = mapX + wallX;
            floorYWall = mapY + 1.0;
        }

        double distWall, distPlayer, currentDist;

        distWall = distance;
        distPlayer = 0.0;

        int drawEnd = lineHeight / 2 + fb.h / 2;
        if (drawEnd < 0) drawEnd = fb.h; //becomes < 0 when the integer overflows
        for (size_t j = drawEnd ; j < fb.h; ++j)
        {
            
            currentDist = fb.h / (2.0 * j - fb.h); //you could make a small lookup table for this instead

            double weight = (currentDist - distPlayer) / (distWall - distPlayer);

            double currentFloorX = weight * floorXWall + (1.0 - weight) * player.get_x();
            double currentFloorY = weight * floorYWall + (1.0 - weight) * player.get_y();

            int floorTexX, floorTexY;
            floorTexX = int(currentFloorX * tex_walls.size) % tex_walls.size;
            floorTexY = int(currentFloorY * tex_walls.size) % tex_walls.size;
            
            //floor
            fb.set_pixel(fb.w / 2 + i, j, tex_walls.get(floorTexX, floorTexY, 0));
            //ceiling
            fb.set_pixel(fb.w/2+i, fb.h - j, pack_color(0, 172, 216));
        }
    }

    draw_map(fb, sprites, tex_walls, map, cell_w, cell_h);
    //std::cout << player.get_angle() << std::endl;
    
    for (size_t i = 0; i < sprites.size(); i++) { // draw the sprites
        draw_sprite(fb, sprites[i], depth_buffer, player, tex_monst);
    }
    
}