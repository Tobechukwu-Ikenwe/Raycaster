#ifndef RAYCASTER_H
#define RAYCASTER_H


#pragma once
#include <cmath>
#include "map.h"
#include "player.h"
#include <vector>


class Raycaster{
    private:
        int width;
        int height;

    public:
    Raycaster(int screenW, int screenH): width(screenW), height(screenH){
        
    }
    std::vector<int> castRays(const Player& p) {
    std::vector<int> columnHeights(width);
    for(int x= 0 ; x < width; ++x){
        double rayAngle = p.angle - Player::FOV /2.0 + (double(x)/width) * Player::FOV;
        
        double rayX = std::cos(rayAngle);
        double rayY = std::sin(rayAngle);

        double distance = 0.0;
        bool hit = false;

        while(!hit && distance < 16.0){
            distance += 0.05;

            int testX = int(p.x + rayX * distance);
            int testY = int(p.y + rayY * distance);

            if (Map::isWall(testX,testY)){
                hit = true;
            }
        }

        // Fix fish eye effect
        double corrected = distance * std::cos(rayAngle - p.angle);
            float wallHeight = float(height / corrected);

            columnHeights[x] = wallHeight;
        }

        return columnHeights;
    }
   
};

#endif
