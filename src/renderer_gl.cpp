#include "renderer_gl.h"
#include "player.h"
#include "map.h"
#include "gl_core.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

const char* kVertSource = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUV;
void main() {
    vUV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* kFragSource = R"(
#version 330 core
in vec2 vUV;
out vec4 fragColor;
uniform vec2 uPlayerPos;
uniform float uPlayerAngle;
uniform float uFov;
uniform vec2 uMapSize;
uniform float uHasKey;
uniform vec2 uResolution;
uniform sampler2D uMapTex;
const float MAX_DEPTH = 20.0;
const float FOG_DIST = 12.0;
const float C_EMPTY = 0.0;
const float C_WALL  = 1.0;
const float C_DOOR  = 2.0;
const float C_KEY   = 3.0;
const float C_EXIT  = 4.0;
float sampleMap(vec2 p) {
    vec2 uv = vec2((p.x + 0.5) / uMapSize.x, 1.0 - (p.y + 0.5) / uMapSize.y);
    return texture(uMapTex, uv).r * 255.0;
}
void main() {
    float aspect = uResolution.x / uResolution.y;
    float rayAngle = uPlayerAngle - uFov * 0.5 + (vUV.x * uFov);
    vec2 dir = vec2(cos(rayAngle), sin(rayAngle));
    float dist = 0.0;
    float cellType = C_EMPTY;
    float stepSize = 0.04;
    for (int i = 0; i < 400; i++) {
        dist += stepSize;
        vec2 pos = uPlayerPos + dir * dist;
        vec2 cell = floor(pos);
        cellType = sampleMap(cell);
        if (cellType >= C_WALL) break;
    }
    if (cellType < C_WALL) {
        vec3 ceilingCol = vec3(70.0/255.0, 130.0/255.0, 180.0/255.0);
        vec3 floorCol = vec3(50.0/255.0, 50.0/255.0, 50.0/255.0);
        vec3 col = mix(floorCol, ceilingCol, step(0.5, vUV.y));
        fragColor = vec4(col, 1.0);
        return;
    }
    vec3 wallCol;
    if (cellType >= C_EXIT - 0.5)
        wallCol = vec3(0.2, 0.6, 0.25);
    else if (cellType >= C_KEY - 0.5)
        wallCol = vec3(0.85, 0.7, 0.2);
    else if (cellType >= C_DOOR - 0.5)
        wallCol = uHasKey > 0.5 ? vec3(0.25, 0.2, 0.15) : vec3(0.35, 0.25, 0.15);
    else
        wallCol = vec3(0.4, 0.35, 0.3);
    float shade = 1.0 - (dist / MAX_DEPTH) * 0.5;
    wallCol *= shade;
    float fog = 1.0 - exp(-dist / FOG_DIST);
    wallCol = mix(wallCol, vec3(0.35, 0.38, 0.4), fog);
    fragColor = vec4(wallCol, 1.0);
}
)";

const char* kMinimapVert = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
out vec2 vUV;
void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* kMinimapFrag = R"(
#version 330 core
in vec2 vUV;
out vec4 fragColor;
uniform vec2 uPlayerPos;
uniform vec2 uMapSize;
uniform float uHasKey;
uniform sampler2D uMapTex;
float sampleMap(vec2 p) {
    vec2 uv = vec2((p.x + 0.5) / uMapSize.x, 1.0 - (p.y + 0.5) / uMapSize.y);
    return texture(uMapTex, uv).r * 255.0;
}
void main() {
    vec2 cell = floor(vec2(vUV.x * uMapSize.x, (1.0 - vUV.y) * uMapSize.y));
    float t = sampleMap(cell);
    vec3 col = vec3(0.2, 0.2, 0.25);
    if (t >= 3.5) col = vec3(0.2, 0.6, 0.25);
    else if (t >= 2.5) col = vec3(0.85, 0.7, 0.2);
    else if (t >= 1.5) col = vec3(0.35, 0.25, 0.15);
    else if (t >= 0.5) col = vec3(0.4, 0.35, 0.3);
    if (floor(cell.x) == floor(uPlayerPos.x) && floor(cell.y) == floor(uPlayerPos.y))
        col = vec3(1.0, 1.0, 1.0);
    fragColor = vec4(col, 1.0);
}
)";

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    if (!id) {
        std::cerr << "glCreateShader failed (OpenGL 3.3 / shaders may not be supported by this context).\n";
        return 0;
    }
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        std::cerr << "Shader compile failed: " << log << "\n";
        glDeleteShader(id);
        return 0;
    }
    return id;
}

} // namespace

RendererGL::~RendererGL() {
    if (solidVbo_) glDeleteBuffers(1, &solidVbo_);
    if (solidVao_) glDeleteVertexArrays(1, &solidVao_);
    if (solidProgram_) glDeleteProgram(solidProgram_);
    if (minimapVbo_) glDeleteBuffers(1, &minimapVbo_);
    if (minimapVao_) glDeleteVertexArrays(1, &minimapVao_);
    if (minimapProgram_) glDeleteProgram(minimapProgram_);
    if (mapTex_) glDeleteTextures(1, &mapTex_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (program_) glDeleteProgram(program_);
}

bool RendererGL::loadShaders() {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, kVertSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, kFragSource);
    if (!vs || !fs) return false;

    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    int success;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
        std::cerr << "Program link failed: " << log << "\n";
        return false;
    }
    return true;
}

bool RendererGL::loadMinimapShaders() {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, kMinimapVert);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, kMinimapFrag);
    if (!vs || !fs) return false;
    minimapProgram_ = glCreateProgram();
    if (!minimapProgram_) { glDeleteShader(vs); glDeleteShader(fs); return false; }
    glAttachShader(minimapProgram_, vs);
    glAttachShader(minimapProgram_, fs);
    glLinkProgram(minimapProgram_);
    glDeleteShader(vs);
    glDeleteShader(fs);
    int success;
    glGetProgramiv(minimapProgram_, GL_LINK_STATUS, &success);
    if (!success) return false;
    // Minimap at bottom center (NDC: x 0.36-0.64, y 0.72-1.0)
    float quad[] = {
        0.36f, 0.72f, 0.0f, 0.0f,
        0.64f, 0.72f, 1.0f, 0.0f,
        0.36f, 1.0f,  0.0f, 1.0f,
        0.36f, 1.0f,  0.0f, 1.0f,
        0.64f, 0.72f, 1.0f, 0.0f,
        0.64f, 1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &minimapVao_);
    glGenBuffers(1, &minimapVbo_);
    glBindVertexArray(minimapVao_);
    glBindBuffer(GL_ARRAY_BUFFER, minimapVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return true;
}

bool RendererGL::loadSolidShaders() {
    const char* vert = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
void main() { gl_Position = vec4(aPos, 0.0, 1.0); }
)";
    const char* frag = R"(
#version 330 core
out vec4 fragColor;
uniform vec3 uColor;
void main() { fragColor = vec4(uColor, 1.0); }
)";
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vert);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, frag);
    if (!vs || !fs) return false;
    solidProgram_ = glCreateProgram();
    if (!solidProgram_) { glDeleteShader(vs); glDeleteShader(fs); return false; }
    glAttachShader(solidProgram_, vs);
    glAttachShader(solidProgram_, fs);
    glLinkProgram(solidProgram_);
    glDeleteShader(vs);
    glDeleteShader(fs);
    int success;
    glGetProgramiv(solidProgram_, GL_LINK_STATUS, &success);
    if (!success) return false;
    float quad[] = { -0.25f,-0.08f, 0.25f,-0.08f, -0.25f,0.08f, -0.25f,0.08f, 0.25f,-0.08f, 0.25f,0.08f };
    glGenVertexArrays(1, &solidVao_);
    glGenBuffers(1, &solidVbo_);
    glBindVertexArray(solidVao_);
    glBindBuffer(GL_ARRAY_BUFFER, solidVbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return true;
}

void RendererGL::uploadMapTexture() {
    unsigned char pixels[Map::height][Map::width];
    for (int y = 0; y < Map::height; y++)
        for (int x = 0; x < Map::width; x++)
            pixels[y][x] = static_cast<unsigned char>(Map::layout[y][x]);

    if (!mapTex_) glGenTextures(1, &mapTex_);
    glBindTexture(GL_TEXTURE_2D, mapTex_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Map::width, Map::height, 0,
                 GL_RED, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool RendererGL::init(int width, int height) {
    winWidth_ = width;
    winHeight_ = height;

    if (!loadShaders()) return false;
    if (!loadMinimapShaders()) return false;
    if (!loadSolidShaders()) return false;

    float quad[] = { -1,-1, 1,-1, -1,1,  -1,1, 1,-1, 1,1 };
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    uploadMapTexture();
    return true;
}

void RendererGL::resize(int width, int height) {
    winWidth_ = width;
    winHeight_ = height;
    glViewport(0, 0, width, height);
}

void RendererGL::draw(const Player& player, bool hasKey, int winWidth, int winHeight) {
    if (winWidth <= 0 || winHeight <= 0) return;
    winWidth_ = winWidth;
    winHeight_ = winHeight;
    glViewport(0, 0, winWidth, winHeight);

    glClearColor(0.1f, 0.12f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_);
    glUniform2f(glGetUniformLocation(program_, "uPlayerPos"), static_cast<float>(player.x), static_cast<float>(player.y));
    glUniform1f(glGetUniformLocation(program_, "uPlayerAngle"), static_cast<float>(player.angle));
    glUniform1f(glGetUniformLocation(program_, "uFov"), 60.0f * 3.14159265f / 180.0f);
    glUniform2f(glGetUniformLocation(program_, "uMapSize"), static_cast<float>(Map::width), static_cast<float>(Map::height));
    glUniform1f(glGetUniformLocation(program_, "uHasKey"), hasKey ? 1.0f : 0.0f);
    glUniform2f(glGetUniformLocation(program_, "uResolution"), static_cast<float>(winWidth), static_cast<float>(winHeight));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTex_);
    glUniform1i(glGetUniformLocation(program_, "uMapTex"), 0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    drawMinimap(player, hasKey, winWidth, winHeight);
}

void RendererGL::drawTitleScreen(int winWidth, int winHeight) {
    if (winWidth <= 0 || winHeight <= 0) return;
    glViewport(0, 0, winWidth, winHeight);
    glClearColor(0.12f, 0.14f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(solidProgram_);
    glUniform3f(glGetUniformLocation(solidProgram_, "uColor"), 0.2f, 0.6f, 0.25f);  // green door
    glBindVertexArray(solidVao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RendererGL::drawWinScreen(int winWidth, int winHeight) {
    if (winWidth <= 0 || winHeight <= 0) return;
    glViewport(0, 0, winWidth, winHeight);
    glClearColor(0.06f, 0.12f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(solidProgram_);
    glUniform3f(glGetUniformLocation(solidProgram_, "uColor"), 0.2f, 0.6f, 0.3f);
    glBindVertexArray(solidVao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RendererGL::drawMinimap(const Player& player, bool hasKey, int winWidth, int winHeight) {
    (void)hasKey;
    (void)winWidth;
    (void)winHeight;
    glUseProgram(minimapProgram_);
    glUniform2f(glGetUniformLocation(minimapProgram_, "uPlayerPos"), static_cast<float>(player.x), static_cast<float>(player.y));
    glUniform2f(glGetUniformLocation(minimapProgram_, "uMapSize"), static_cast<float>(Map::width), static_cast<float>(Map::height));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTex_);
    glUniform1i(glGetUniformLocation(minimapProgram_, "uMapTex"), 0);
    glBindVertexArray(minimapVao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
