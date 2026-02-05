#ifndef RENDERER_GL_H
#define RENDERER_GL_H

class Player;

class RendererGL {
public:
    RendererGL() = default;
    ~RendererGL();

    bool init(int width, int height);
    void draw(const Player& player, bool hasKey, int winWidth, int winHeight);
    void drawTitleScreen(int winWidth, int winHeight);
    void drawWinScreen(int winWidth, int winHeight);
    void resize(int width, int height);

private:
    unsigned int program_ = 0;
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int mapTex_ = 0;
    unsigned int minimapProgram_ = 0;
    unsigned int minimapVao_ = 0;
    unsigned int minimapVbo_ = 0;
    unsigned int solidProgram_ = 0;
    unsigned int solidVao_ = 0;
    unsigned int solidVbo_ = 0;
    int winWidth_ = 0;
    int winHeight_ = 0;

    bool loadShaders();
    bool loadMinimapShaders();
    bool loadSolidShaders();
    void uploadMapTexture();
    void drawMinimap(const Player& player, bool hasKey, int winWidth, int winHeight);
};

#endif // RENDERER_GL_H
