
#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>

#include "framebuffer.h"
#include "utils.h"

FrameBuffer::FrameBuffer()
    : w(1024), h(512), img(1024 * 512, pack_color(255, 255, 255)) {}

void FrameBuffer::set_pixel(const short x, const short y, const uint32_t color) {
    //assert(img.size() == w * h && x < w && y < h);
    img[x + y * w] = color;
}

void FrameBuffer::draw_rectangle(const short rect_x, const short rect_y, const short rect_w, const short rect_h, const uint32_t color) {
    assert(img.size() == w * h);
    for (short i = 0; i < rect_w; i++) {
        for (short j = 0; j < rect_h; j++) {
            short cx = rect_x + i;
            short cy = rect_y + j;
            if (cx < w && cy < h) // no need to check for negative values (unsigned variables)
                set_pixel(cx, cy, color);
        }
    }
}

void FrameBuffer::clear(const uint32_t color) {
    img = std::vector<uint32_t>(w * h, color);
}


