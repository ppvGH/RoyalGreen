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
#include "core/shader.h"
#include "core/resource_manager.h"
#include "core/camera.h"
#include "app/scene.h"
#include "graphics/model.h"

//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>  //////////////////////////////////////////stbimage
//

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

const std::string pathModel = std::string(MESHES_DIR) + "tempArcade.obj";


int main()
{
    GLFWwindow* window = initGLFWwindow(width, height, "Royal Green");
    
    if(!initGLAD(window)) return -1;

    glEnable(GL_DEPTH_TEST);



    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    
    
    


    // shaders
    Shader& basic = ResourceManager::loadShader(pathVert, pathFrag, "basic");

    // Scene camera test callbacks TODO
    Scene scene;
    glfwSetWindowUserPointer(window, &scene);
    Callbacks::initCallbacks(window);

    Model arcadeCabinet;
    arcadeCabinet.loadModel(pathModel);


    // Model matrix.
    glm::mat4 model = glm::mat4(1.0);

    

    // Models customized for cubes.
    glm::mat4 model1 = glm::scale(model, glm::vec3(20.0));

    try
    {
        //std::cout << arcadeCabinet.getMesh("cabinet_display").m_matIndex << std::endl;
    }
    catch (const std::runtime_error e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    /* Render Loop. */
    
    while (!glfwWindowShouldClose(window))
    {
        /* Camera input management. */
        glfwPollEvents();
        handleCameraInput(window, &scene);
        
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            Mesh& powerButton = arcadeCabinet.getMesh("power_button_power_button");
            Material& matPowerButton = arcadeCabinet.m_materials[powerButton.m_matIndex];
            matPowerButton.m_emission = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        

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

        /* ImGui frame. */
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



        /* Scene render. */
        arcadeCabinet.draw(basic);
 
        
        glfwSwapBuffers(window);
       
    }

    // 4. Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
