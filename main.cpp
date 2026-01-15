#include <iostream>
#include <vector>

// NOTE: Unused
#include <thread>
// NOTE: Unused
#include <chrono>

#include <SDL2/SDL.h> 
#include "map.h"

// NOTE:
// What are the flags needed to compile?

// BUG: Needs to be ranamed can not find raycaster.h
#include "raycaster.h"
#include "player.h"
//clang++ main.cpp -o raycaster `pkg-config --cflags --libs sdl2`

constexpr int screenW = 1280;
constexpr int screenH = 720;

int main(int argc, char* argv[]){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // NOTE: Recommendation
    //  Lets start using smart ptrs when you can. A good rule of thumb always use smart ptrs over 'dumb ptrs' when you can
    SDL_Window *window = SDL_CreateWindow("Raycaster 3D - SDL2", 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                            screenW, screenH, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player player;
    Raycaster raycaster(screenW,screenH);

    Uint32 lastTime = SDL_GetTicks();
    bool running = true;
    SDL_Event event;

    while(running){
        Uint32 currentTime = SDL_GetTicks();
        double deltaTime = (currentTime - lastTime) / 1000.0; 
        lastTime = currentTime; 
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        double moveSpeed = 3.0 * deltaTime;
        double rotSpeed = 2.0 * deltaTime;

        if (state[SDL_SCANCODE_W]) {
            double dx = std::cos(player.angle) * moveSpeed;
            double dy = std::sin(player.angle) * moveSpeed;
            // Add a 0.1 buffer in the direction of movement
            // Note: Use static cast instead static_cast<int> over c style cast
            // Note: Please move player.x over to a new line hard to read
            if (!Map::isWall((int)(player.x + dx + (dx > 0 ? 0.1 : -0.1)), (int)player.y))
              player.x += dx;
            if (!Map::isWall((int)player.x, (int)(player.y + dy + (dy > 0 ? 0.1 : -0.1))))
              player.y += dy;
        }
        
        if (state[SDL_SCANCODE_S]) {
            double dx = std::cos(player.angle) * moveSpeed;
            double dy = std::sin(player.angle) * moveSpeed;
            if (!Map::isWall((int)(player.x - dx - (dx > 0 ? 0.1 : -0.1)), (int)player.y)) player.x -= dx;
            if (!Map::isWall((int)player.x, (int)(player.y - dy - (dy > 0 ? 0.1 : -0.1)))) player.y -= dy;
        }

        if (state[SDL_SCANCODE_A]) player.angle -= rotSpeed;
        if (state[SDL_SCANCODE_D]) player.angle += rotSpeed;
        if (state[SDL_SCANCODE_ESCAPE]) running = false;

        // Clear Screen (Background Colors)
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Dark Gray Ceiling
        SDL_RenderClear(renderer);

        // Clear Screen (Background Colors)
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); // Lighter Gray Floor
        SDL_Rect floorRect = {0, screenH / 2, screenW, screenH / 2};
        SDL_RenderFillRect(renderer, &floorRect);
        
        auto walls = raycaster.castRays(player);

    
        for (int x = 0; x < screenW; ++x) {
            float wallHeight = walls[x];
            SDL_FRect wallSlice;
            wallSlice.x = (float)x;
            wallSlice.w = 1.0f; // Width of one slice
            wallSlice.h = wallHeight;
            wallSlice.y = (screenH - wallHeight) / 2.0f;

            int brightness = 255 - (wallHeight / 4); 
            if (brightness < 50) brightness = 50;
            if (brightness > 255) brightness = 255;

            SDL_SetRenderDrawColor(renderer, brightness, 0, 0, 255);    

            int y_start = (screenH - wallHeight) / 2;
            int y_end = (screenH + wallHeight) / 2;

            SDL_RenderDrawLine(renderer, x, y_start, x, y_end);
        }
          
        //Update screen
        SDL_RenderPresent(renderer);
        
        }
        //cleanup
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}
    
