// NOTE: This is redundant.
//      You are using a header guard + pragma once 
//      all you need is pragma once decide which one you prefer to use
#ifndef MAP_H
#define MAP_H

#pragma once
#include<vector>

// TIP: Keeping everything header and compiling it to one big binary is okay for smaller projects
// However in bigger projects this will slow down compilation a lot as if you make a minor change the whole binary 
// will need to be recompiled. A good tip is to start splitting things up into different folders and compile into a binary.
// Then link it during compilation this way the whole binary does not have to be recompiled if you change something
// Create a map.h map.c then compile into a map.o then link it with main.o. Look into static linking

class Map{
    static constexpr int width = 16;
    static constexpr int height = 16;

    //1 = wall 0 = empty

    static constexpr int grid[height][width] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,1,1,1,0,1,1,1,0,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,0,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
        {1,0,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
        {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    public:
    // Note: this can be rewritten better you can simply just say return grid[y][x] == 1 instead of conditions
    // Tip: this is a optimization if you know the compiler will never throw add noexcept
    static bool isWall(int x, int y) noexcept {
        return grid[y][x] == 1;
    }
};

#endif
