#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>


#include "core/callbacks.h"



int main()
{
    // 1. Inizializza GLFW
    if (!glfwInit())
    {
        std::cerr << "Errore: impossibile inizializzare GLFW\n";
        return -1;
    }

    // Versione OpenGL (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crea finestra
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Royal Green", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Errore: impossibile creare la finestra GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Carica funzioni OpenGL con GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Errore: impossibile inizializzare GLAD\n";
        return -1;
    }

    // Initialize callbacks
    Callbacks::initCallbacks(window);


    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // 3. Loop principale
    while (!glfwWindowShouldClose(window))
    {

        // Rendering OpenGL
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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
