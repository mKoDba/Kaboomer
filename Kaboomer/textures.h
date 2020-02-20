#pragma once

#include <vector>
#include <cstdint>
#include <string>

class Texture {
public:
    size_t img_w, img_h; // overall image dimensions
    size_t count, size;  // number of textures and size in pixels
    std::vector<uint32_t> img; // textures storage container

    Texture(const std::string filename, const uint32_t format);
    uint32_t get(const short i, const short j, const short idx) const; // get the pixel (i,j) from the texture idx
    std::vector<uint32_t> get_scaled_column(const short texture_id, const short tex_coord, const short column_height) const; // retrieve one column (tex_coord) from the texture texture_id and scale it to the destination size
};
