#ifndef PLAYER_H
#define PLAYER_H


#pragma once
#include<cmath>

struct Player{
    double x = 1.5;
    double y = 1.5;
    double angle = 0.0;

    static constexpr double FOV = M_PI/3.0;
};


#endif
