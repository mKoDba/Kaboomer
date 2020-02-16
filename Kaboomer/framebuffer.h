#pragma once

#include <cstddef>
#include <cstdlib>
#include <vector>

class FrameBuffer {
public:
    size_t w, h; // image dimensions
    std::vector<uint32_t> img; // storage container

    FrameBuffer();
    void clear(const uint32_t color);
    void set_pixel(const size_t x, const size_t y, const uint32_t color);
    void draw_rectangle(const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color);
};
