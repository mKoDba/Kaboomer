#pragma once

#include <cstdlib>

class Map {
public:
    size_t w, h; // overall map dimensions
    int get(const size_t i, const size_t j) const;
    bool is_empty(const size_t i, const size_t j) const;

    Map();
};
