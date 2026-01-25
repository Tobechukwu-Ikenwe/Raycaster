#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <SDL2/SDL.h>

// Project headers are intentionally minimal and decoupled.
// Each component owns a single responsibility:
//  - Player: camera state
//  - Raycaster: visibility + projection math
//  - Map: world representation and collision queries
#include "player.h"
#include "raycaster.h"
#include "map.h"

// Fixed resolution to keep CPU cost deterministic.
// This makes performance characteristics measurable and predictable.
constexpr int SCREEN_WIDTH  = 1280;
constexpr int SCREEN_HEIGHT = 720;

/*
    Game
    ----
    High-level orchestration layer.

    This class intentionally avoids rendering or math logic.
    Its sole responsibility is:
      - lifecycle management
      - input dispatch
      - frame pacing
      - delegating work to subsystems

    This separation mirrors how larger engines scale:
    logic stays testable, rendering stays swappable.
*/
class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    void processInput(double deltaTime);
    void render();

    // SDL owns the platform abstraction (window, vsync, presentation).
    // The engine logic remains platform-agnostic.
    SDL_Window* window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    // Core simulation state
    Player player_;
    Raycaster raycaster_;

    bool running_ = true;
};

// Raycaster is constructed with explicit resolution.
// This avoids hidden global state and allows easy resolution scaling.
Game::Game() : raycaster_(SCREEN_WIDTH, SCREEN_HEIGHT) {}

Game::~Game() {
    // Explicit teardown order makes ownership clear
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_)   SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Game::initialize() {
    // SDL handles OS-level initialization; failure here is unrecoverable
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Window creation is intentionally minimal — no hidden OpenGL context.
    // This keeps CPU-side rendering behavior transparent.
    window_ = SDL_CreateWindow(
        "Level6 Raycaster - SDL2 CPU Demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window_) return false;

    // Hardware-accelerated presentation + vsync.
    // Rendering remains CPU-driven; SDL handles buffer swap efficiently.
    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    return renderer_ != nullptr;
}

/*
    processInput
    ------------
    Input is processed in terms of *intent*, not raw key presses.

    Movement is frame-rate independent via deltaTime.
    Collision is queried through the Map interface, not hardcoded logic.
*/
void Game::processInput(double deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running_ = false;
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    // Tuned movement constants scaled by frame delta
    double moveSpeed = 3.0 * deltaTime;
    double rotSpeed  = 2.0 * deltaTime;

    // Direction vector derived from player angle
    double dx = std::cos(player_.angle) * moveSpeed;
    double dy = std::sin(player_.angle) * moveSpeed;

    // Forward / backward movement with collision queries
    if (state[SDL_SCANCODE_W]) {
        if (!Map::isWall(static_cast<int>(player_.x + dx),
                         static_cast<int>(player_.y)))
            player_.x += dx;

        if (!Map::isWall(static_cast<int>(player_.x),
                         static_cast<int>(player_.y + dy)))
            player_.y += dy;
    }

    if (state[SDL_SCANCODE_S]) {
        if (!Map::isWall(static_cast<int>(player_.x - dx),
                         static_cast<int>(player_.y)))
            player_.x -= dx;

        if (!Map::isWall(static_cast<int>(player_.x),
                         static_cast<int>(player_.y - dy)))
            player_.y -= dy;
    }

    // Rotation is decoupled from translation
    if (state[SDL_SCANCODE_A]) player_.angle -= rotSpeed;
    if (state[SDL_SCANCODE_D]) player_.angle += rotSpeed;

    if (state[SDL_SCANCODE_ESCAPE]) running_ = false;
}

/*
    render
    ------
    Rendering is intentionally explicit and linear.

    Each vertical screen column corresponds to exactly one ray.
    This mirrors how the algorithm maps cleanly to a GPU fragment shader
    without hiding the math behind abstractions.
*/
void Game::render() {
    // Ceiling (background)
    SDL_SetRenderDrawColor(renderer_, 70, 130, 180, 255);
    SDL_RenderClear(renderer_);

    // Floor
    SDL_SetRenderDrawColor(renderer_, 50, 50, 50, 255);
    SDL_Rect floorRect {
        0,
        SCREEN_HEIGHT / 2,
        SCREEN_WIDTH,
        SCREEN_HEIGHT / 2
    };
    SDL_RenderFillRect(renderer_, &floorRect);

    // Raycasting produces one projected wall slice per column
    auto walls = raycaster_.castRays(player_);

    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        float wallHeight = walls[x];

        int yStart = static_cast<int>((SCREEN_HEIGHT - wallHeight) / 2.0f);
        int yEnd   = static_cast<int>((SCREEN_HEIGHT + wallHeight) / 2.0f);

        // Distance-based shading to reinforce depth perception
        int brightness = std::clamp(
            255 - static_cast<int>(wallHeight * 2),
            50,
            255
        );

        SDL_SetRenderDrawColor(
            renderer_,
            brightness,
            brightness / 2,
            brightness / 2,
            255
        );

        SDL_RenderDrawLine(renderer_, x, yStart, x, yEnd);
    }

    SDL_RenderPresent(renderer_);
}

/*
    run
    ---
    Main loop uses fixed responsibilities:
      - measure time
      - process input
      - render frame

    No logic leaks across layers.
*/
void Game::run() {
    Uint32 lastTime = SDL_GetTicks();

    while (running_) {
        Uint32 currentTime = SDL_GetTicks();
        double deltaTime =
            static_cast<double>(currentTime - lastTime) / 1000.0;
        lastTime = currentTime;

        processInput(deltaTime);
        render();
    }
}

/*
    Entry point
    -----------
    Ownership is explicit and exception-safe.
    main() does not contain engine logic.
*/
int main() {
    auto game = std::make_unique<Game>();

    if (!game->initialize())
        return 1;

    game->run();
    return 0;
}
