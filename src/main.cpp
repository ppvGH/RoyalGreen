#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>

#include <fstream>  //std::ifstream
#include <sstream>  //std::sstream

#include <filesystem> // std::filesystem::current_path(), useful for debug

#include "core/callbacks.h"
#include "core/utilities.h"
#include "core/shader.h"

// Runs on GPU
extern "C"
{
    _declspec(dllexport) int NvOptimusEnablement = 1;
}

/* Paths to shader source files.
 * SHADERS_DIR has been defined in the CMakeLists.txt file as
 * SHADERS_DIR = SHADERS_DIR="${CMAKE_SOURCE_DIR}/res/shaders" */
const std::string pathVert = std::string(SHADERS_DIR) + "basic.vert";
const std::string pathFrag = std::string(SHADERS_DIR) + "basic.frag";




std::string loadShaderFromFile(const std::string& shPath)
{
    // inputfilestream -> stringstream -> string

    // string container to receive the string stream
    std::string shCode;

    try
    {
        // open file with inputfilestream
        std::ifstream shFile(shPath);

        // create a stringstream
        std::stringstream shStream;

        // read file's buffer content into stream
        shStream << shFile.rdbuf();

        // close file handler
        shFile.close();

        // convert stream into string
        shCode = shStream.str();

    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR::SHADER: File reading failure" << std::endl;
    }
    return shCode;
}




int main()
{
    GLFWwindow* window = initGLFWwindow(800, 800, "Royal Green");
    
    if(!initGLAD(window)) return -1;

    // Initialize callbacks
    Callbacks::initCallbacks(window);


    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // testing shaders with fake vbo, vao
    float vertices[] = {
        // pos      
        0.0f, 1.0f, 
        1.0f, 0.0f, 
        0.0f, 0.0f, 

        0.0f, 1.0f, 
        1.0f, 1.0f, 
        1.0f, 0.0f
    };


    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    


    // shaders
    Shader shader;
    const std::string srcVert = loadShaderFromFile(pathVert);
    const std::string srcFrag = loadShaderFromFile(pathFrag);
    shader.linkProgram(srcVert, srcFrag);
    shader.use();

    // 3. Loop principale
    while (!glfwWindowShouldClose(window))
    {

        // Rendering OpenGL
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //// Start frame ImGui
        //ImGui_ImplOpenGL3_NewFrame();
        //ImGui_ImplGlfw_NewFrame();
        //ImGui::NewFrame();

        //// UI di test
        //ImGui::Begin("Demo Window");
        //ImGui::Text("Ciao da Royal Green!");
        //ImGui::End();

        //// Render UI
        //ImGui::Render();
        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glDrawArrays(GL_TRIANGLES, 0, 6);


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
