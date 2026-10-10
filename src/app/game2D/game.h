#pragma once
#include "../../graphics/framebuffer.h"
#include "sprite_renderer.h"
#include "background.h"
#include "gameHUD.h"
#include "game_session.h"
class Game {
public:
    Game(int width,int height);
    void resetGame() { m_session.reset(); }
    void input2DHandler(const ActionMap& actions,float dt) { m_session.input(actions,dt); }
    void update(float dt) { m_session.update(dt); }
    void render() const;
    const SpriteRenderer& getSpriteRenderer() const { return m_spriteRenderer; }
    const Texture& getFBOTex() const { return m_FBO.getTex(); }
    float getCamPos() const { return m_session.cameraX(); }
    int getCatLives() const { return m_session.cat().getLives(); }
    bool isGameMenuOpen() const { return m_session.isMenuOpen(); }
    void closeGameMenu() { m_session.closeMenu(); }
    bool isHelpOverlayOn() const { return m_session.help(); }
    void switchHelpOverlay() { m_session.toggleHelp(); }
private:
    int m_width,m_height;
    GameSession m_session;
    Background m_background;
    GameHUD m_gameHUD;
    Framebuffer m_FBO;
    SpriteRenderer m_spriteRenderer;
    Shader& m_shader;
};
