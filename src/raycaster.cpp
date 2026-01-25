#include "raycaster.h"
#include "player.h"
#include "map.h"
#include <cmath>

Raycaster::Raycaster(int screenWidth, int screenHeight)
    : screenWidth_(screenWidth), screenHeight_(screenHeight) {}

std::vector<float> Raycaster::castRays(const Player& player) {
    std::vector<float> walls(screenWidth_);

    for (int x = 0; x < screenWidth_; ++x) {
        float rayAngle = (player.angle - fov_/2.0f) + (x / static_cast<float>(screenWidth_)) * fov_;
        float distanceToWall = 0.0f;
        bool hitWall = false;

        double eyeX = std::cos(rayAngle);
        double eyeY = std::sin(rayAngle);

        while (!hitWall && distanceToWall < maxDepth_) {
            distanceToWall += 0.05f;

            int testX = static_cast<int>(player.x + eyeX * distanceToWall);
            int testY = static_cast<int>(player.y + eyeY * distanceToWall);

            if (Map::isWall(testX, testY)) hitWall = true;
        }

        walls[x] = (screenHeight_ / (distanceToWall + 0.0001f)) * 2.0f;
    }

    return walls;
}
