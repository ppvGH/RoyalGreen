#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include <filesystem> // std::filesystem::current_path(), useful for debug
#include <stdexcept> //runtimeerror

#include "core/callbacks.h"
#include "core/utilities.h"
#include "core/resource_manager.h"  // contains shader.h and texture.h too
#include "graphics/camera.h"
#include "app/scene.h"
#include "graphics/model.h"
#include "graphics/framebuffer.h"


/* Runs on GPU (only nvidia for now). */
extern "C"
{
    _declspec(dllexport) int NvOptimusEnablement = 1;
}



constexpr int width = 800;
constexpr int height = 800;

/* Paths to shader source files. 
 * <X>_DIR are in the CMakeLists.txt, note that they end with a / character.
 * SHADERS_DIR = "${CMAKE_SOURCE_DIR}/res/shaders/" 
 * MESHES_DIR = "${CMAKE_SOURCE_DIR}/res/meshes/" */
const std::string pathVert = std::string(SHADERS_DIR) + "basic.vert";
const std::string pathFrag = std::string(SHADERS_DIR) + "basic.frag";
const std::string path2DVert = std::string(SHADERS_DIR) + "test2D.vert";
const std::string path2DFrag = std::string(SHADERS_DIR) + "test2D.frag";

const std::string pathModel = std::string(MESHES_DIR) + "tempArcade.obj";

const std::string pathRospi = std::string(TEXTURE_DIR) + "rospi.png";

float getDeltaTime(float& lastFrame)
{
    float currentFrame = static_cast<float>(glfwGetTime()); // tempo attuale in secondi
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}



int main()
{

    GLFWwindow* window = initGLFWwindow(width, height, "Royal Green");
    
    if(!initGLAD(window)) return -1;

    glEnable(GL_DEPTH_TEST);


    //imgui stuff
    {
        // 2. Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    // shaders
    Shader& basic = ResourceManager::loadShader(pathVert, pathFrag, "basic");
    Shader& test2D = ResourceManager::loadShader(path2DVert, path2DFrag, "test2D");

    // texture
    Texture& rospi = ResourceManager::loadTexture(pathRospi, TexParams(), "rospi");

    // Scene camera test callbacks TODO
    Scene scene;
    //glfwSetWindowUserPointer(window, &scene);
    Callbacks::initCallbacks(window);

    // Model loading.
    Model arcadeCabinet;
    arcadeCabinet.loadModel(pathModel);
    // Model matrix.
    glm::mat4 model = glm::mat4(1.0);

    
    //////////////////////////////// TEST 2D///////////////////////////////////////////

    //float quadVerts[] = {
    //    // pos          // uv 
    //    0.0f, 0.0f,     0.0f, 0.0f,
    //    512.0f, 0.0f,   0.0f, 1.0f,
    //    512.0f, 512.0f, 1.0f, 1.0f,

    //    0.0f, 0.0f,     0.0f, 0.0f,
    //    512.0f, 512.0f, 1.0f, 1.0f,
    //    0.0f, 512.0f,   1.0f, 0.0f
    //};

    //// Screen TO THE GPU
    //GLuint VAO, VBO;
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);

    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //glBindVertexArray(0);

    //// ORTHO PROJ
    //glm::mat4 ortho = glm::ortho(0.0f, 512.0f, 0.0f, 512.0f);
    //

    //// DRAW ON FBO
    //// FRAMEBUFFER
    //GLuint fbo;
    //glGenFramebuffers(1, &fbo);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glViewport(0, 0, 512, 512);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    //glActiveTexture(GL_TEXTURE0);
    //rospi.bind();
    //test2D.use();
    //test2D.setInt("imageTex", 0);
    //test2D.setMatrix4fv("projection", 1, GL_FALSE, ortho);

    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //// RESTORE VIEWPORT
    //glViewport(0, 0, width, height);
    ////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////// TEST 2D DYNAMIC //////////////////////////////////////////////

    constexpr int screenWidth = 512;
    constexpr int screenHeight = 512;

    //// Texture vuota
    //Texture screenTex(TexParams(screenWidth, screenHeight), nullptr);

    //// FBO
    //GLuint fbo;
    //glGenFramebuffers(1, &fbo);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex.getID(), 0);

    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cerr << "FBO non completo!" << std::endl;

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Framebuffer screenFBO(screenWidth, screenHeight);

    float triVerts[] = {
        // pos         // color
        0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        50.0f, 100.0f, 0.0f, 1.0f, 0.0f,
        100.0f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVerts), triVerts, GL_STATIC_DRAW);

    // posizione
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // colore
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    float triX = 0.0f;
    float speed = 100.0f; // pixel/sec
    bool right = true;
    float lastFrame = 0.0f;

    /* Render Loop. *////////////////////////////////////////////////////////////////////////
    
    while (!glfwWindowShouldClose(window))
    {
        /* Camera input management. */
        glfwPollEvents();
        handleCameraInput(window, &scene);
        
        /* Setting shaders and camera. */
        basic.use();
        basic.setMatrix4fv("model", 1, GL_FALSE, model);
        basic.setMatrix4fv("view", 1, GL_FALSE, scene.cam3D.getViewMatrix());
        basic.setMatrix4fv("proj", 1, GL_FALSE, scene.cam3D.getPerspectiveProjMatrix());
        /* Setting the lights. */
        basic.setVector3f("lightPos", glm::vec3(20.0f));
        basic.setVector3f("viewPos", scene.cam3D.getPosition());

        /* OpenGL stuff. */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        /*inputs*/
        {
            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
            {
                Mesh& powerButton = arcadeCabinet.getMesh("power_button_power_button");
                Material& matPowerButton = arcadeCabinet.m_materials[powerButton.m_matIndex];
                matPowerButton.m_emission = glm::vec3(1.0f, 0.0f, 0.0f);
            }
        }
        

       

        

        /* ImGui frame. */
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            /* ImGui demo UI setup. */
            ImGui::Begin("Demo Window");
            ImGui::Text("Ciao da Royal Green!");
            ImGui::End();
            /* ImGui render. */
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }


        
        ////////////////////////////////// FBO RENDER TEST //////////////////////////////////////////
        float dt = getDeltaTime(lastFrame); // calcola deltaTime tra frame

        // aggiorna posizione
        if (right) { triX += speed * dt; if (triX + 100 > screenWidth) right = false; }
        else { triX -= speed * dt; if (triX < 0) right = true; }

        // ---- Render nel FBO ----
        screenFBO.bind();
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        test2D.use();
        glm::mat4 proj = glm::ortho(0.0f, float(screenWidth), 0.0f, float(screenHeight));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(triX, 100, 0.0f));
        test2D.setMatrix4fv("projection", 1, GL_FALSE, proj * model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        

        /* Scene render. */


        Mesh& screen = arcadeCabinet.getMesh("cabinet_display");
        Material& matScreen = arcadeCabinet.m_materials[screen.m_matIndex];
        matScreen.m_overrideTex = &screenFBO.getTex();
        matScreen.m_useTex = true;
        basic.use();
        arcadeCabinet.draw(basic);

        
        glfwSwapBuffers(window);
       
    }

    // 4. Cleanup ImGui
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
