#include "game.h"
#include "../scene3D/scene_data.h"
#include "game_data.h"
#include "../../core/resource_manager.h"
#include <glm/gtc/matrix_transform.hpp>
Game::Game(int width,int height):m_width(width),m_height(height),m_session(width,height),
    m_background(gameData::backgroundTexName,gameData::backgroundPosition,gameData::backgroundSize),
    m_gameHUD(gameData::catLifeTexName),m_FBO(width,height,sceneData::FBOtypeColor),
    m_spriteRenderer(width,height),m_shader(ResourceManager::getShader(gameData::gameShaderName)) {}

void Game::render() const
{
    /* Framebuffer and viewport setup. */
    m_FBO.bind();
    glViewport(0, 0, m_FBO.getWidth(), m_FBO.getHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* Camera setup. */
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    m_shader.use();
    m_shader.setMatrix4fv("view", 1, GL_FALSE, view);

    /* Rendering game elements. */
    m_background.render(m_spriteRenderer, m_shader);

    m_session.player().render(m_spriteRenderer, m_shader);

    m_session.cat().render(m_spriteRenderer, m_shader);

    m_session.projectiles().render(m_spriteRenderer, m_shader);

    /* TODO: adjust HUD. */
    glm::vec2 firstLifePosition = glm::vec2(m_width - gameData::catLifeSize.x - 10.0f, 550.0f);
    for(int i = 0; i < m_session.cat().getLives(); i++)
    {
        glm::vec2 catLifePosition = firstLifePosition - glm::vec2(static_cast<float>(i) * gameData::catLifeSize.x + 5.0f, 0.0f);
        m_gameHUD.render(m_spriteRenderer, m_shader, catLifePosition, gameData::catLifeSize);
    }



    /* Detaches the FBO. */
    m_FBO.unbind();
}
