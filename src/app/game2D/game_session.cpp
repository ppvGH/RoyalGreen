#include "game_session.h"
#include "game_data.h"
#include "../../core/validation.h"
GameSession::GameSession(int width,int height):m_width(width),m_height(height),
    m_player(gameData::playerTexName,gameData::playerPosition,gameData::playerSize,gameData::playerVelocity),
    m_cat(gameData::catTexName,gameData::catPosition,gameData::catSize,gameData::catVelocity),
    m_collisions(m_player,m_cat,m_projectiles) {
    validation::require(width>=gameData::playerSize.x && width>=gameData::catSize.x && height>gameData::groundLevel,
        "Arena dimensions must contain the characters");
    m_player.constrainToArena(width); m_cat.constrainToArena(width);
}
void GameSession::input(const ActionMap& actions,float dt) {
    validation::time(dt);
    if(actions.justStarted(Action::HelpOverlay2D)) toggleHelp();
    if(actions.justStarted(Action::GameMenu)) m_menu=true;
    if(m_menu) return;
    m_player.inputHandler(actions,dt,m_projectiles);
    m_cat.autoInput(dt,m_player.getPosition().x);
}
void GameSession::update(float dt) {
    validation::time(dt);
    if(m_menu) return;
    if(m_player.getState()==State::Dead || m_cat.getState()==State::Dead) { m_menu=true; return; }
    m_player.update(dt,m_projectiles); m_cat.update(dt,m_projectiles,m_player.getPosition(),0);
    m_player.constrainToArena(m_width); m_cat.constrainToArena(m_width);
    m_projectiles.update(dt,static_cast<float>(m_width),-gameData::arrowSize.x,static_cast<float>(m_height),0);
    m_collisions.checkCollision();
    if(m_player.getState()==State::Dead || m_cat.getState()==State::Dead) m_menu=true;
}
void GameSession::reset() {
    m_projectiles.resetProjSys(); m_player.resetPlayer(); m_cat.resetCat();
    m_player.constrainToArena(m_width); m_cat.constrainToArena(m_width); m_menu=false;
}
