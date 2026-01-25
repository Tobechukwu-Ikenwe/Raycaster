#ifndef MAP_H
#define MAP_H

#include <array>

class Map {
public:
    static bool isWall(int x, int y);

    static constexpr int width  = 16;
    static constexpr int height = 16;

    static const std::array<std::array<int, width>, height> layout;
};

#endif // MAP_H
