#include "game.h"

#include "../scene3D/scene_data.h"
#include "game_data.h"

#include "../../core/resource_manager.h"
#include "../../core/commonGL.h"
#include <glm/gtc/matrix_transform.hpp> //glm::translate
#include <iostream>

Game::Game(int width, int height) :
    m_width(width),
    m_height(height),
    m_background(gameData::backgroundTexName, gameData::backgroundPosition, gameData::backgroundSize),
    m_projectilesSystem(),
    m_player(gameData::playerTexName, gameData::playerPosition, gameData::playerSize, gameData::playerVelocity),
    m_cat(gameData::catTexName, gameData::catPosition, gameData::catSize, gameData::catVelocity),
    m_gameHUD(gameData::catLifeTexName),
    m_collisionSystem(m_player, m_cat, m_projectilesSystem),
	m_FBO(width, height, sceneData::FBOtypeColor),
	m_spriteRenderer(width, height),
    m_shader(ResourceManager::getShader(gameData::gameShaderName)),
    m_cameraPosition(m_player.getPosition().x),
    m_cameraMargin(m_player.getSize().x * 2.0f),
    m_gameMenuOpen(false)
{
}


void Game::resetGame()
{
    /* Reset player. */
    m_player.resetPlayer();

    /* Reset enemy. */
    m_cat.resetCat();

    /* Reset projectile manager. */
    m_projectilesSystem.resetProjSys();

    /* Reset camera. */
    m_cameraPosition = m_player.getPosition().x;
}



void Game::input2DHandler(const ActionMap& actionMap2D, float dt)
{
    /* Game Inputs. */
    appInputHandler(actionMap2D);

    /* Player Inputs. */
    m_player.inputHandler(actionMap2D, dt, m_projectilesSystem);

    /* Cat (automatic) inputs. */
    m_cat.autoInput(dt, m_player.getPosition().x);

}




void Game::update(float dt)
{

    if (m_player.getState() != State::Dead && m_cat.getState() != State::Dead)
    {
        updateCamera();

        m_player.update(dt, m_projectilesSystem);
        m_cat.update(dt, m_projectilesSystem, m_player.getPosition(), m_cameraPosition);


        /* Updates projectiles position and if they are out of camera view it removes them. */
        float rightLim = m_cameraPosition + m_width;
        float leftLim = m_cameraPosition - gameData::arrowSize.x;
        float topLim = m_height;
        float botLim = 0.0f;
        m_projectilesSystem.update(dt, rightLim, leftLim, topLim, botLim);

        /* Collision system update. */
        m_collisionSystem.checkCollision();
    }
    else
    {
        openGameMenu();
    }


    
}



void Game::render() const
{
    /* Framebuffer and viewport setup. */
    m_FBO.bind();
    glViewport(0, 0, m_FBO.getWidth(), m_FBO.getHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* Camera setup. */
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_cameraPosition, 0.0f, 0.0f));
    m_shader.use();
    m_shader.setMatrix4fv("view", 1, GL_FALSE, view);

    /* Rendering game elements. */
    m_background.render(m_spriteRenderer, m_shader);
    m_background.render(m_spriteRenderer, m_shader, 2.0f * gameData::backgroundPosition);

    m_player.render(m_spriteRenderer, m_shader);

    m_cat.render(m_spriteRenderer, m_shader);

    m_projectilesSystem.render(m_spriteRenderer, m_shader);

    /* TODO: adjust HUD. */
    glm::vec2 firstLifePosition = glm::vec2(m_cameraPosition + m_width - gameData::catLifeSize.x - 10.0f, 550.0f);
    for(int i = 0; i < m_cat.getLives(); i++)
    {
        glm::vec2 catLifePosition = firstLifePosition - glm::vec2(static_cast<float>(i) * gameData::catLifeSize.x + 5.0f, 0.0f);
        m_gameHUD.render(m_spriteRenderer, m_shader, catLifePosition, gameData::catLifeSize);
    }



    /* Detaches the FBO. */
    m_FBO.unbind();
}


void Game::updateCamera()
{
    /* Right border: (P > C + (W - M)) ---> C = P - (W - M). */
    /* Left border:  (P < C + M )    ------> C = P - M. */
    if (m_player.getPosition().x > m_cameraPosition + m_width - m_cameraMargin)
        m_cameraPosition = m_player.getPosition().x - (m_width - m_cameraMargin);
    else if (m_player.getPosition().x < m_cameraPosition + m_cameraMargin)
        m_cameraPosition = m_player.getPosition().x - m_cameraMargin;

    /* Bounds cam position to be non-negative. */
    if (m_cameraPosition < 0.0f) m_cameraPosition = 0.0f;
}


void Game::appInputHandler(const ActionMap& actionMap2D)
{
    if (actionMap2D.justStarted(Action::GameMenu)) openGameMenu();
}