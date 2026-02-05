/*
 * FIND THE DOOR — First-person maze game with raycasting
 * ------------------------------------------------------
 * Main loop: input -> game logic -> render.
 * Maze: fixed 2D grid (map.h/map.cpp). Raycasting: GPU (GL) or CPU (raycaster.cpp).
 * UI: timer (countdown), elapsed time, score on screen; minimap at bottom.
 */
#define SDL_MAIN_HANDLED
#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <string>
#include <SDL2/SDL.h>

#include "gl_core.h"
#include "player.h"
#include "map.h"
#include "renderer_gl.h"
#include "raycaster.h"

// Resolution: GL path high-res; CPU fallback lower for ~60 FPS.
constexpr int SCREEN_WIDTH  = 2560;
constexpr int SCREEN_HEIGHT = 1440;
constexpr int CPU_WIDTH     = 1280;
constexpr int CPU_HEIGHT    = 720;
constexpr double TIMER_START = 120.0;   // Countdown seconds to reach exit
constexpr double MOUSE_SENSITIVITY = 0.003;  // Radians per pixel for mouse look

class Game {
public:
    Game() : raycaster_(CPU_WIDTH, CPU_HEIGHT) {}
    ~Game();

    bool initialize();
    void run();

private:
    void processInput(double deltaTime);
    void render();
    void renderTitleScreen();
    void renderTitleScreenCPU();
    void renderGL();
    void renderCPU();
    void renderMinimapCPU();
    void checkPickups();
    void updateTitle();

    SDL_Window* window_   = nullptr;
    SDL_GLContext glContext_ = nullptr;
    SDL_Renderer* sdlRenderer_ = nullptr;
    RendererGL rendererGL_;
    Raycaster raycaster_;

    Player player_;
    bool hasKey_ = false;
    bool hasWon_ = false;
    bool hasLost_ = false;
    bool running_ = true;
    bool useCpuRenderer_ = false;
    bool showTitleScreen_ = true;
    double timer_ = TIMER_START;
    double elapsedTime_ = 0.0;  // Seconds since game start (for "time taken" on win)
    int score_ = 0;
    static constexpr int MINIMAP_CELL = 8;
    static constexpr int MINIMAP_MARGIN = 8;

    void drawBlockText(SDL_Renderer* r, const char* text, int cx, int cy, int blockW, int blockH, int gap);
    void drawBlockTextLeft(SDL_Renderer* r, const char* text, int x, int y, int blockW, int blockH, int gap);
    std::string formatTime(double seconds) const;
    void renderWinScreenCPU();
    void renderWinScreenGL();
};

Game::~Game() {
    if (glContext_) SDL_GL_DeleteContext(glContext_);
    if (sdlRenderer_) SDL_DestroyRenderer(sdlRenderer_);
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window_ = SDL_CreateWindow(
        "Dungeon Run — Find the key, reach the exit",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window_) return false;

    glContext_ = SDL_GL_CreateContext(window_);
    if (!glContext_) {
        std::cerr << "OpenGL context failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        goto use_cpu;
    }

    SDL_GL_SetSwapInterval(1);
    if (gl_core_load() != 0) {
        std::cerr << "OpenGL function loader failed.\n";
        SDL_GL_DeleteContext(glContext_);
        SDL_DestroyWindow(window_);
        glContext_ = nullptr;
        window_ = nullptr;
        goto use_cpu;
    }

    if (!rendererGL_.init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "OpenGL 3.3 not available; using CPU raycaster fallback (1280x720).\n";
        SDL_GL_DeleteContext(glContext_);
        SDL_DestroyWindow(window_);
        glContext_ = nullptr;
        window_ = nullptr;
        goto use_cpu;
    }
    SDL_SetWindowTitle(window_,
        "Find the GREEN door | Get key first, pass brown door | SPACE to start");
    return true;

use_cpu:
    useCpuRenderer_ = true;
    window_ = SDL_CreateWindow(
        "Dungeon Run — Find the key, reach the exit (CPU)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CPU_WIDTH,
        CPU_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window_) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }
    sdlRenderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer_) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }
    SDL_SetWindowTitle(window_,
        "Find the GREEN door | Get key first, pass brown door | SPACE to start");
    return true;
}

void Game::checkPickups() {
    if (hasWon_ || hasLost_) return;
    int px = static_cast<int>(player_.x);
    int py = static_cast<int>(player_.y);
    int cell = Map::getCell(px, py);
    if (cell == Cell::Key && !hasKey_) hasKey_ = true;
    if (cell == Cell::Exit && hasKey_) {
        hasWon_ = true;
        score_ = static_cast<int>(timer_);
    }
}

void Game::updateTitle() {
    if (showTitleScreen_) return;
    std::string title = "Dungeon Run";
    if (hasWon_) {
        title += " — You escaped! Score: " + std::to_string(score_);
    } else if (hasLost_) {
        title += " — Time's up! Score: 0";
    } else {
        int sec = static_cast<int>(timer_) % 60;
        int min = static_cast<int>(timer_) / 60;
        title += " — " + std::to_string(min) + ":" + (sec < 10 ? "0" : "") + std::to_string(sec);
        if (hasKey_) title += " [KEY]";
    }
    SDL_SetWindowTitle(window_, title.c_str());
}

void Game::processInput(double deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running_ = false;
        if (!useCpuRenderer_ && event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
            rendererGL_.resize(event.window.data1, event.window.data2);
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (showTitleScreen_) {
        if (state[SDL_SCANCODE_SPACE]) {
            showTitleScreen_ = false;
            elapsedTime_ = 0.0;
            SDL_SetRelativeMouseMode(SDL_TRUE);  // Mouse look
            updateTitle();
        }
        return;
    }

    // --- Game mechanics: timer (countdown), elapsed time, score ---
    if (!hasWon_ && !hasLost_) {
        timer_ -= deltaTime;
        elapsedTime_ += deltaTime;
        if (timer_ <= 0.0) {
            timer_ = 0.0;
            hasLost_ = true;
        }
    }

    // --- Player movement: first-person, collision detection ---
    // W/S = forward/backward, A/D = strafe left/right, mouse = rotate view
    double moveSpeed = 3.0 * deltaTime;
    double strafeSpeed = 2.5 * deltaTime;
    double dx = std::cos(player_.angle) * moveSpeed;
    double dy = std::sin(player_.angle) * moveSpeed;
    double perpX = -std::sin(player_.angle) * strafeSpeed;
    double perpY =  std::cos(player_.angle) * strafeSpeed;

    auto tryMove = [this](double nx, double ny) {
        int ix = static_cast<int>(nx);
        int iy = static_cast<int>(ny);
        if (!Map::isBlocking(ix, iy, hasKey_)) {
            player_.x = nx;
            player_.y = ny;
        }
    };

    if (!hasLost_ && !hasWon_) {
        if (state[SDL_SCANCODE_W]) {
            tryMove(player_.x + dx, player_.y);
            tryMove(player_.x, player_.y + dy);
        }
        if (state[SDL_SCANCODE_S]) {
            tryMove(player_.x - dx, player_.y);
            tryMove(player_.x, player_.y - dy);
        }
        if (state[SDL_SCANCODE_A]) {
            tryMove(player_.x + perpX, player_.y);
            tryMove(player_.x, player_.y + perpY);
        }
        if (state[SDL_SCANCODE_D]) {
            tryMove(player_.x - perpX, player_.y);
            tryMove(player_.x, player_.y - perpY);
        }
    }
    if (state[SDL_SCANCODE_ESCAPE]) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        running_ = false;
    }

    checkPickups();
    updateTitle();
}

// Simple 5x7 block font: 1 = on. Each char is 5 cols, 7 rows. Space = empty.
static const unsigned char kBlockFont[][35] = {
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0}, // F
    {0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}, // I
    {1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // N
    {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}, // D
    {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, // space
    {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}, // T
    {1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // H
    {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // E
    {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, // space
    {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // O
    {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,0,1,0, 1,0,0,0,1}, // R
    {1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // W
    {0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}, // I
    {1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // N
    {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}, // !
    {0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // A
    {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}, // B
    // Digits 0-9 for timer and score
    {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // 0
    {0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}, // 1
    {0,1,1,1,0, 1,0,0,0,1, 0,0,0,0,1, 0,0,1,1,0, 0,1,0,0,0, 1,0,0,0,0, 1,1,1,1,1}, // 2
    {1,1,1,1,0, 0,0,0,0,1, 0,0,0,1,0, 0,0,1,1,0, 0,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // 3
    {0,0,0,1,0, 0,0,1,1,0, 0,1,0,1,0, 1,0,0,1,0, 1,1,1,1,1, 0,0,0,1,0, 0,0,0,1,0}, // 4
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,0, 0,0,0,0,1, 0,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // 5
    {0,1,1,1,0, 1,0,0,0,0, 1,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // 6
    {1,1,1,1,1, 0,0,0,0,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 0,1,0,0,0}, // 7
    {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // 8
    {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 0,1,1,1,1, 0,0,0,0,1, 0,0,0,0,1, 0,1,1,1,0}, // 9
    {0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,0,0,0}, // :
    {1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,1}, // L
    {0,1,1,1,0, 1,0,0,0,0, 0,1,1,0,0, 0,0,0,1,0, 0,0,0,0,1, 1,0,0,0,1, 0,1,1,1,0}, // S
    {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,1, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}, // -
    {1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1}, // M
    {0,1,1,1,0, 1,0,0,0,1, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,1, 0,1,1,1,0}, // C
};
static int blockCharIndex(char c) {
    if (c >= '0' && c <= '9') return 17 + (c - '0');
    switch (c) {
        case 'F': return 0; case 'I': return 1; case 'N': return 2; case 'D': return 3;
        case ' ': return 4; case 'T': return 5; case 'H': return 6; case 'E': return 7;
        case 'O': return 9; case 'R': return 10; case 'W': return 11; case '!': return 14;
        case 'A': return 15; case 'B': return 16; case ':': return 27; case 'L': return 28;
        case 'S': return 29; case '-': return 30; case 'M': return 31; case 'C': return 32;
        default: return 4;
    }
}

void Game::drawBlockText(SDL_Renderer* r, const char* text, int cx, int cy, int blockW, int blockH, int gap) {
    int len = 0;
    for (const char* p = text; *p; p++) len++;
    int totalW = len * (5 * blockW + gap) - gap;
    int x = cx - totalW / 2;
    int y = cy - (7 * blockH) / 2;
    for (const char* p = text; *p; p++) {
        int idx = blockCharIndex(*p);
        if (idx >= 0 && idx < 33) {
            const unsigned char* g = kBlockFont[idx];
            for (int row = 0; row < 7; row++)
                for (int col = 0; col < 5; col++)
                    if (g[row * 5 + col]) {
                        SDL_Rect rect = { x + col * blockW, y + row * blockH, blockW, blockH };
                        SDL_RenderFillRect(r, &rect);
                    }
        }
        x += 5 * blockW + gap;
    }
}

void Game::drawBlockTextLeft(SDL_Renderer* r, const char* text, int x, int y, int blockW, int blockH, int gap) {
    for (const char* p = text; *p; p++) {
        int idx = blockCharIndex(*p);
        if (idx >= 0 && idx < 33) {
            const unsigned char* g = kBlockFont[idx];
            for (int row = 0; row < 7; row++)
                for (int col = 0; col < 5; col++)
                    if (g[row * 5 + col]) {
                        SDL_Rect rect = { x + col * blockW, y + row * blockH, blockW, blockH };
                        SDL_RenderFillRect(r, &rect);
                    }
        }
        x += 5 * blockW + gap;
    }
}

std::string Game::formatTime(double seconds) const {
    int s = static_cast<int>(seconds) % 60;
    int m = static_cast<int>(seconds) / 60;
    return std::to_string(m) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
}

void Game::renderTitleScreenCPU() {
    int w = CPU_WIDTH, h = CPU_HEIGHT;
    SDL_SetRenderDrawColor(sdlRenderer_, 20, 22, 35, 255);
    SDL_RenderClear(sdlRenderer_);
    SDL_SetRenderDrawColor(sdlRenderer_, 255, 220, 100, 255);
    int blockW = 14, blockH = 18, gap = 6;
    drawBlockText(sdlRenderer_, "FIND THE GREEN DOOR", w / 2, h / 2 - 40, blockW, blockH, gap);
    SDL_SetRenderDrawColor(sdlRenderer_, 180, 180, 200, 255);
    drawBlockText(sdlRenderer_, "Get key -> brown door -> green exit", w / 2, h / 2 + 50, 5, 7, 2);
    SDL_SetRenderDrawColor(sdlRenderer_, 50, 180, 80, 255);
    SDL_Rect doorSample = { w/2 - 60, h/2 + 95, 120, 36 };
    SDL_RenderFillRect(sdlRenderer_, &doorSample);
    SDL_SetRenderDrawColor(sdlRenderer_, 255, 220, 100, 255);
    drawBlockText(sdlRenderer_, "SPACE START", w / 2, h - 50, 6, 8, 3);
    SDL_RenderPresent(sdlRenderer_);
}

void Game::renderWinScreenGL() {
    int w, h;
    SDL_GL_GetDrawableSize(window_, &w, &h);
    rendererGL_.drawWinScreen(w, h);
    SDL_GL_SwapWindow(window_);
}

void Game::renderWinScreenCPU() {
    int w = CPU_WIDTH, h = CPU_HEIGHT;
    SDL_SetRenderDrawColor(sdlRenderer_, 15, 25, 15, 255);
    SDL_RenderClear(sdlRenderer_);
    SDL_SetRenderDrawColor(sdlRenderer_, 80, 255, 120, 255);
    drawBlockText(sdlRenderer_, "YOU FOUND THE GREEN DOOR", w / 2, h / 2 - 50, 10, 12, 3);
    drawBlockText(sdlRenderer_, "YOU WIN!", w / 2, h / 2 + 20, 14, 16, 5);
    SDL_SetRenderDrawColor(sdlRenderer_, 200, 220, 200, 255);
    std::string timeTaken = "TIME: " + formatTime(elapsedTime_);
    drawBlockText(sdlRenderer_, timeTaken.c_str(), w / 2, h / 2 + 80, 8, 10, 2);
    std::string scoreStr = "SCORE: " + std::to_string(score_);
    drawBlockText(sdlRenderer_, scoreStr.c_str(), w / 2, h / 2 + 110, 8, 10, 2);
    SDL_RenderPresent(sdlRenderer_);
}

void Game::renderTitleScreen() {
    if (useCpuRenderer_)
        renderTitleScreenCPU();
    else {
        int w, h;
        SDL_GL_GetDrawableSize(window_, &w, &h);
        rendererGL_.drawTitleScreen(w, h);
        SDL_GL_SwapWindow(window_);
    }
}

void Game::renderGL() {
    int w, h;
    SDL_GL_GetDrawableSize(window_, &w, &h);
    rendererGL_.draw(player_, hasKey_, w, h);
    SDL_GL_SwapWindow(window_);
}

// --- UI: minimap at bottom center to help user find the door ---
void Game::renderMinimapCPU() {
    const int mapPx = Map::width * MINIMAP_CELL;
    const int mapPy = Map::height * MINIMAP_CELL;
    const int mx = (CPU_WIDTH - mapPx) / 2;
    const int my = CPU_HEIGHT - MINIMAP_MARGIN - mapPy;

    SDL_Rect bg = { mx - 2, my - 2, mapPx + 4, mapPy + 4 };
    SDL_SetRenderDrawColor(sdlRenderer_, 20, 20, 30, 230);
    SDL_RenderFillRect(sdlRenderer_, &bg);
    SDL_SetRenderDrawColor(sdlRenderer_, 80, 80, 100, 255);
    SDL_RenderDrawRect(sdlRenderer_, &bg);

    for (int cy = 0; cy < Map::height; ++cy)
        for (int cx = 0; cx < Map::width; ++cx) {
            int c = Map::layout[cy][cx];
            Uint8 r = 60, g = 60, b = 60;
            if (c == Cell::Wall) { r = 90; g = 85; b = 80; }
            else if (c == Cell::Door) { r = 100; g = 70; b = 50; }
            else if (c == Cell::Key) { r = 220; g = 180; b = 40; }
            else if (c == Cell::Exit) { r = 50; g = 180; b = 80; }
            SDL_Rect cell = { mx + cx * MINIMAP_CELL, my + cy * MINIMAP_CELL, MINIMAP_CELL, MINIMAP_CELL };
            SDL_SetRenderDrawColor(sdlRenderer_, r, g, b, 255);
            SDL_RenderFillRect(sdlRenderer_, &cell);
        }

    int px = mx + static_cast<int>(player_.x * MINIMAP_CELL);
    int py = my + static_cast<int>(player_.y * MINIMAP_CELL);
    SDL_SetRenderDrawColor(sdlRenderer_, 255, 255, 255, 255);
    SDL_Rect playerRect = { px - 1, py - 1, 3, 3 };
    SDL_RenderFillRect(sdlRenderer_, &playerRect);
}

void Game::renderCPU() {
    SDL_SetRenderDrawColor(sdlRenderer_, 70, 130, 180, 255);
    SDL_RenderClear(sdlRenderer_);

    SDL_SetRenderDrawColor(sdlRenderer_, 50, 50, 50, 255);
    SDL_Rect floorRect{ 0, CPU_HEIGHT / 2, CPU_WIDTH, CPU_HEIGHT / 2 };
    SDL_RenderFillRect(sdlRenderer_, &floorRect);

    // --- Raycasting renderer: walls with distance shading (depth effect) ---
    auto walls = raycaster_.castRays(player_, hasKey_);
    for (int x = 0; x < CPU_WIDTH; ++x) {
        float wallHeight = walls[x];
        int yStart = static_cast<int>((CPU_HEIGHT - wallHeight) / 2.0f);
        int yEnd   = static_cast<int>((CPU_HEIGHT + wallHeight) / 2.0f);
        int brightness = std::clamp(255 - static_cast<int>(wallHeight * 2), 50, 255);
        SDL_SetRenderDrawColor(sdlRenderer_, brightness, brightness / 2, brightness / 2, 255);
        SDL_RenderDrawLine(sdlRenderer_, x, yStart, x, yEnd);
    }

    // --- UI: timer and score on screen (top-left) ---
    const int uiBlockW = 5, uiBlockH = 7, uiGap = 2;
    const int uiY = MINIMAP_MARGIN;
    SDL_SetRenderDrawColor(sdlRenderer_, 255, 255, 200, 255);
    std::string timeStr = "TIME: " + formatTime(elapsedTime_);
    drawBlockTextLeft(sdlRenderer_, timeStr.c_str(), MINIMAP_MARGIN, uiY, uiBlockW, uiBlockH, uiGap);
    std::string timerStr = "LEFT: " + formatTime(timer_);
    drawBlockTextLeft(sdlRenderer_, timerStr.c_str(), MINIMAP_MARGIN, uiY + 30, uiBlockW, uiBlockH, uiGap);
    std::string scoreStr = "Score: " + (hasWon_ ? std::to_string(score_) : (hasLost_ ? "0" : "-"));
    drawBlockTextLeft(sdlRenderer_, scoreStr.c_str(), MINIMAP_MARGIN, uiY + 60, uiBlockW, uiBlockH, uiGap);

    renderMinimapCPU();
    SDL_RenderPresent(sdlRenderer_);
}

void Game::render() {
    if (showTitleScreen_) {
        renderTitleScreen();
        return;
    }
    if (hasWon_) {
        if (useCpuRenderer_)
            renderWinScreenCPU();
        else
            renderWinScreenGL();
        return;
    }
    if (useCpuRenderer_)
        renderCPU();
    else
        renderGL();
}

/*
 * Main loop: handle input, game logic, rendering. Target ~60 FPS with vsync.
 */
void Game::run() {
    Uint32 lastTime = SDL_GetTicks();

    while (running_) {
        Uint32 currentTime = SDL_GetTicks();
        double deltaTime = static_cast<double>(currentTime - lastTime) / 1000.0;
        lastTime = currentTime;

        processInput(deltaTime);
        render();
    }
}

/*
 * Entry point: initialize (window, GL or CPU renderer, maze), then run main loop.
 */
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    auto game = std::make_unique<Game>();

    if (!game->initialize())
        return 1;

    game->run();
    return 0;
}
