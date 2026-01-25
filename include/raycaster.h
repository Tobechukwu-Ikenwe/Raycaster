#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <vector>
class Player;

class Raycaster {
public:
    Raycaster(int screenWidth, int screenHeight);
    std::vector<float> castRays(const Player& player);

private:
    int screenWidth_;
    int screenHeight_;
    float fov_ = 60.0f * 3.14159265f / 180.0f;
    float maxDepth_ = 16.0f;
};

#endif // RAYCASTER_H
