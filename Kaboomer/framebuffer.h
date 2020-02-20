#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>

class FrameBuffer {
public:
    short w, h; // image dimensions
    std::vector<uint32_t> img; // storage container

    FrameBuffer();
    void clear(const uint32_t color);
    void set_pixel(const short x, const short y, const uint32_t color);
    void draw_rectangle(const short x, const short y, const short w, const short h, const uint32_t color);
};
