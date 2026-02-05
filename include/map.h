#ifndef MAP_H
#define MAP_H

#include <array>

/*
 * Maze world: fixed 2D grid. Each cell is either empty or wall (or special: door, key, exit).
 * Used for collision detection and by the raycasting renderer.
 */
namespace Cell { enum { Empty = 0, Wall = 1, Door = 2, Key = 3, Exit = 4 }; }

class Map {
public:
    static bool isBlocking(int x, int y, bool hasKey);  // Collision: true if player cannot walk through
    static int getCell(int x, int y);

    static constexpr int width  = 24;
    static constexpr int height = 24;

    static const std::array<std::array<int, width>, height> layout;  // Maze data structure
};

#endif // MAP_H
