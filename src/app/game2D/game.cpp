#include "game.h"

#include "../scene_data.h"
#include "game_data.h"

#include "../../core/resource_manager.h"
#include "../../core/commonGL.h"
#include <glm/gtc/matrix_transform.hpp> //glm::translate
#include <iostream>

Game::Game(int width, int height) :
    m_width(width),
    m_height(height),
    m_player(gameData::playerTexName, gameData::playerPosition, gameData::playerSize, 0.0f),
    m_background(gameData::backgroundTexName, gameData::backgroundPosition, gameData::backgroundSize),
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
    m_player.resetPlayer();
    m_cameraPosition = m_player.getPosition().x;
}



void Game::input2DHandler(const ActionMap& actionMap2D, float dt)
{
    appInputHandler(actionMap2D);

    m_player.inputHandler(actionMap2D, dt);
}




void Game::update(float dt)
{
    updateCamera();

    m_player.update(dt);
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


    /* Detaches the FBO. */
    m_FBO.unbind();
}


void Game::updateCamera()
{
    /* Right border: (P > C + (W - P)) ---> C = P - (W - P). */
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