#include "map.h"

/*
 * Maze layout: fixed 2D grid. 0=empty, 1=wall, 2=locked door, 3=key, 4=exit.
 * Player starts near (3,3). One special cell is the exit door; get key first to pass the locked door.
 */
const std::array<std::array<int, Map::width>, Map::height> Map::layout = {{
    {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,1,3,0,1,0,0,0,0,0,1}},  // 3 = key
    {{1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,1,1,2,1,0,0,0,0,0,1}},   // 2 = locked door
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
    {{1,0,0,0,0,0,0,4,0,0,0,0,0,0,0,1}},   // 4 = exit
    {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}}
}};

int Map::getCell(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return Cell::Wall;
    return layout[y][x];
}

bool Map::isBlocking(int x, int y, bool hasKey) {
    if (x < 0 || x >= width || y < 0 || y >= height) return true;
    int c = layout[y][x];
    if (c == Cell::Wall) return true;
    if (c == Cell::Door) return !hasKey;  // door opens when you have the key
    return false;
}
