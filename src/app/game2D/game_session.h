#pragma once
#include "player.h"
#include "cat.h"
#include "projectile_manager.h"
#include "collision_system.h"
// CPU encounter coordinator. No window, framebuffer, shader or draw calls.
// Texture names are resolved by the actors; CPU tests inject ID-zero textures.
class GameSession {
public:
    GameSession(int width,int height);
    // CollisionSystem borrows these actors; moving/copying would detach its references.
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
    GameSession(GameSession&&) = delete;
    GameSession& operator=(GameSession&&) = delete;
    void input(const ActionMap&,float dt);
    void update(float dt);
    void reset();
    bool isMenuOpen() const { return m_menu; }
    void closeMenu() { m_menu=false; }
    bool help() const { return m_help; }
    void toggleHelp() { m_help=!m_help; }
    Player& player() { return m_player; }
    const Player& player() const { return m_player; }
    Cat& cat() { return m_cat; }
    const Cat& cat() const { return m_cat; }
    ProjectileManager& projectiles() { return m_projectiles; }
    const ProjectileManager& projectiles() const { return m_projectiles; }
    float cameraX() const { return 0.0f; }
private:
    int m_width,m_height;
    ProjectileManager m_projectiles;
    Player m_player;
    Cat m_cat;
    CollisionSystem m_collisions;
    bool m_menu=false,m_help=true;
};
