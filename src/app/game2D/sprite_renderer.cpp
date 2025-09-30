#include "sprite_renderer.h"
// class included in .cpp, just fwd declared in .h
#include "../../graphics/shader.h"
#include "../../graphics/texture.h" // da togliere quando creo classe sprite TODO

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>



SpriteRenderer::SpriteRenderer(int width, int height):
    m_quadVAO(0),
    m_width(width),
    m_height(height)
{
    initData();
}

void SpriteRenderer::drawSprite(Shader& shader, Texture& texture, glm::vec2 position, glm::vec2 size, glm::vec4 uvCoords) const
{

    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position,1.0));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    
    /* Enable the shader to set its uniform.*/
    shader.use();
    shader.setMatrix4fv("projection", 1, GL_FALSE, proj);
    shader.setMatrix4fv("model", 1, GL_FALSE, model);
    shader.setVector4f("uvCoords", uvCoords);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindVertexArray(0);
}

void SpriteRenderer::initData()
{
    float quadVerts[] =
    {
        // pos          // tex
        0.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f,

        1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 1.0f,     1.0f, 1.0f

    };

    GLuint VBO;
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // tex
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
