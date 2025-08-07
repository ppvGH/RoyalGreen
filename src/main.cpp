#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include <filesystem> // std::filesystem::current_path(), useful for debug

#include "core/callbacks.h"
#include "core/utilities.h"
#include "core/shader.h"
#include "core/resource_manager.h"
#include "core/camera.h"

// Runs on GPU (only nvidia for now)
extern "C"
{
    _declspec(dllexport) int NvOptimusEnablement = 1;
}



constexpr int width = 800;
constexpr int height = 800;

/* Paths to shader source files.
 * SHADERS_DIR has been defined in the CMakeLists.txt file as
 * SHADERS_DIR = SHADERS_DIR="${CMAKE_SOURCE_DIR}/res/shaders" */
const std::string pathVert = std::string(SHADERS_DIR) + "basic.vert";
const std::string pathFrag = std::string(SHADERS_DIR) + "basic.frag";









int main()
{
    GLFWwindow* window = initGLFWwindow(width, height, "Royal Green");
    
    if(!initGLAD(window)) return -1;

    glEnable(GL_DEPTH_TEST);

    // Initialize callbacks
    Callbacks::initCallbacks(window);


    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    struct Cube {
        // testing shaders with fake vbo, vao
        float vertices[24] =
        {
            // x      y     z
            // top face
            -0.5f, -0.5f,  0.5f,          // 0
             0.5f, -0.5f,  0.5f,          // 1 
             0.5f,  0.5f,  0.5f,          // 2
            -0.5f,  0.5f,  0.5f,          // 3
            // bottom face                // 4
            -0.5f, -0.5f, -0.5f,          // 5
             0.5f, -0.5f, -0.5f,          // 6
             0.5f,  0.5f, -0.5f,          // 7
            -0.5f,  0.5f, -0.5f,          // 8
        };

        unsigned int indices[36] =
        {
            // 1st triangle   2nd triangle
                1, 5, 0,        5, 4, 0,      // left face
                1, 5, 2,        5, 6, 2,      // front face
                2, 6, 3,        6, 7, 3,      // right face
                0, 4, 3,        4, 7, 3,      // back face
                0, 1, 3,        1, 2, 3,      // top face
                4, 5, 7,        5, 6, 7       // bottom face

        };
    };
    
    Cube testCube1, testCube2;
    

    unsigned int VAO1, VBO1, EBO1;
    unsigned int VAO2, VBO2, EBO2;

    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);

    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testCube1.vertices), testCube1.vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(testCube1.indices), testCube1.indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testCube2.vertices), testCube2.vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(testCube2.indices), testCube2.indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);


    // shaders
    Shader& basic = ResourceManager::loadShader(pathVert, pathFrag, "basic");

    // Camera setup.
    glm::vec3 eye = glm::vec3(21.0f);
    glm::vec3 at = glm::vec3(0.0f);
    Camera cam(eye, at, 45.0f, float(width) / float(height), 10.0f, 100.0f);

    // Model matrix.
    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(2));

    // setting uniforms for shader
    basic.use();
    basic.setUniformMatrix4fv("model", 1, GL_FALSE, model);
    basic.setUniformMatrix4fv("view", 1, GL_FALSE, cam.getViewMatrix());
    basic.setUniformMatrix4fv("proj", 1, GL_FALSE, cam.getPerspectiveProjMatrix());

    // Models customized for cubes.
    glm::mat4 model1 = glm::translate(model, glm::vec3(2.0, 0.0, 0.0));
    glm::mat4 model2 = glm::translate(model, glm::vec3(-2.0, 0.0, 0.0));
    model2 = glm::scale(model2, glm::vec3(2.0));

    

    // 3. Loop principale
    int n = 0;
    float dt = 0.0f;
    float lastFrameTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        // Time stuff.
        float currentFrameTime = glfwGetTime();
        dt = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        static int frameCount = 0;
        static float fpsTimer = 0.0f;
        frameCount++;
        fpsTimer += dt;

        if (fpsTimer >= 1.0f)
        {
            //std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        // Camera management.
        //cam.setTarget(glm::vec3(2*glm::cos(currentFrameTime), 2*glm::sin(currentFrameTime), 0.0));
        //basic.setUniformMatrix4fv("view", 1, GL_FALSE, cam.getViewMatrix());

        // Rendering OpenGL
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI di test
        ImGui::Begin("Demo Window");
        ImGui::Text("Ciao da Royal Green!");
        ImGui::End();

        // Render UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        // Rendering.
        basic.use();

        glBindVertexArray(VAO1);
        model1 = glm::translate(model1, glm::vec3(0.1*glm::cos(currentFrameTime), 0.1*glm::sin(currentFrameTime), 0.0));
        basic.setUniformMatrix4fv("model", 1, GL_FALSE, model1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO2);
        basic.setUniformMatrix4fv("model", 1, GL_FALSE, model2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 4. Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
