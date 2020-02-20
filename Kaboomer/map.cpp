#include <cstdlib>
#include <cassert>

#include "map.h"

const char map[] =  "2222222222222222"\
                    "2              2"\
                    "2      44444   2"\
                    "2     4        2"\
                    "2     4  4444442"\
                    "2     4        2"\
                    "2  444444      2"\
                    "2   4   44444  2"\
                    "2   4          2"\
                    "2   4   4  44442"\
                    "2       4      2"\
                    "2   44444      2"\
                    "2       4      2"\
                    "2  444444444   2"\
                    "2              2"\
                    "2222222222222222"; // game map


// 16x16 map for simplicity
Map::Map() : w(16), h(16) {
    assert(sizeof(map) == w * h + 1); // +1 for the null terminated string
}

// get char (OF WALL) of hardcoded map defined up here
int Map::get(const size_t i, const size_t j) const{
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] - '0';
}

// NO wall check
bool Map::is_empty(const size_t i, const size_t j) const{
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] == ' ';
}

