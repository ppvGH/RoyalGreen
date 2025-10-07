#include "core/commonGL.h"

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
#include "graphics/model.h"
#include "app/scene.h"
#include "app/ui_manager.h"
#include "app/arcade.h"
#include "app/game2D/framebuffer.h"
#include "app/game2D/sprite_renderer.h"
#include "app/game2D/game.h"

#include "core/input_manager.h"
#include "core/action_map.h"

#include "app/path.h"             // contains all assets filepath
#include "app/game2D/game_data.h" // contains game related data

/* Runs on GPU (only nvidia for now). */
extern "C"
{
    _declspec(dllexport) int NvOptimusEnablement = 1;
}

/* IF CUSTOM SIZE */
//constexpr int width = 1000;
//constexpr int height = 1000;

void assetLoader()
{
    // shaders
    ResourceManager::loadShader(Path::pathVert, Path::pathFrag, "basic");   // for 3D stuff
    ResourceManager::loadShader(Path::path2DVert, Path::path2DFrag, "basic2D"); // for the aim
    ResourceManager::loadShader(Path::pathTex2DVert, Path::pathTex2DFrag, "tex2D"); // for the 2D game

    // texture for 3D scene
    ResourceManager::loadTexture(Path::pathRospi, TexParams(), "rospi");
    ResourceManager::loadTexture(Path::pathFloorTile, TexParams(), "floorTile");

    // texture for 2D game
    ResourceManager::loadTexture(Path::pathPlayerSprite, TexParams(), gameData::playerTexName);
    ResourceManager::loadTexture(Path::pathBackground, TexParams(), gameData::backgroundTexName);
}

void keyRegistering(InputManager& input)
{
    input.registerKey(GLFW_KEY_W);
    input.registerKey(GLFW_KEY_A);
    input.registerKey(GLFW_KEY_S);
    input.registerKey(GLFW_KEY_D);

   // input.registerKey(GLFW_KEY_H);
    //input.registerKey(GLFW_KEY_K);
    input.registerKey(GLFW_KEY_M);
    input.registerKey(GLFW_MOUSE_BUTTON_LEFT);

}


void keyBindings3D(ActionMap& actionMap)
{
    actionMap.bind(Action::MoveForward, GLFW_KEY_W);
    actionMap.bind(Action::MoveLeft, GLFW_KEY_A);
    actionMap.bind(Action::MoveBackward, GLFW_KEY_S);
    actionMap.bind(Action::MoveRight, GLFW_KEY_D);

    //actionMap.bind(Action::StartAnimation, GLFW_KEY_K);
    //actionMap.bind(Action::SwitchScreen, GLFW_KEY_H);
    actionMap.bind(Action::SelectObject, GLFW_MOUSE_BUTTON_LEFT);
}

void keyBindings2D(ActionMap& actionMap)
{
    actionMap.bind(Action::P1Jump, GLFW_KEY_W);
    actionMap.bind(Action::P1MoveLeft, GLFW_KEY_A);
    actionMap.bind(Action::P1Attack, GLFW_KEY_S);
    actionMap.bind(Action::P1MoveRight, GLFW_KEY_D);

    actionMap.bind(Action::GameMenu, GLFW_KEY_M);
}


int main()
{
    /* GLFW init and window with display ratio. */
    GLFWwindow* window = initGLFWwindow("Royal Green", 0.7);
    //GLFWwindow* window = initGLFWwindow(800,800,"Royal Green");  // window with custom size
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* GUI. */
    UIManager ui;
    ui.init(window);

    /* Retrieving window size and setting its center. */
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    
    
    /* GLAD init and depth test enabling. */
    if(!initGLAD(window)) return -1;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // #########################################################################
    // ###########################  Loading assets  ############################
    // #########################################################################
    assetLoader();
    
    // #########################################################################
    // ############################  Inputs init  ##############################
    // #########################################################################

    InputManager input;
    keyRegistering(input); 

    ActionMap action3DMap(input);
    keyBindings3D(action3DMap);

    ActionMap action2DMap(input);
    keyBindings2D(action2DMap);

    // #########################################################################
    // ###########################  Scene and Game  ############################
    // #########################################################################
    // Generating scene
    Scene scene(width, height);

    /* Callbacks. (after scene declaration) */
    Callbacks::initCallbacks(window);

    /* 2D Game rendered into the FBO. */

    constexpr int screenWidth = 600;
    constexpr int screenHeight = 600;
    Game gameTest(screenWidth, screenHeight);
    

    // #########################################################################
    // #############################  Render Loop  #############################
    // #########################################################################
    
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        /* Polling. */
        glfwPollEvents();


        /* Camera3D initialized. */
        scene.initCam3D();

        // #########################################################################
        // ############################ Inputs managing ############################
        // #########################################################################
        float dt = getDeltaTime(lastFrame); 


        /* Cursor to the center of the window and camera inputs. 
         * When the animation is on camera inputs are disabled, and
         * cursorCentered is set to false so the cursor gets centered again
         * when the animation is finished, because cursor pos can still change during animation. */
        input.update(window);

        if (ui.isMenuOpen()) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            if (scene.isInput3DEnabled()) scene.input3DHandler(window, action3DMap);
            else if (scene.isInput2DEnabled()) gameTest.input2DHandler(action2DMap, dt);
        }
        

        /*GUI*/
        ui.beginFrame();    
        //if (ImGui::GetIO().WantCaptureKeyboard) std::cout << "capture\n";
        ui.drawUI(scene, gameTest, width, height);
        

        // #########################################################################
        // ############### 2D rendering into the FrameBuffer Object ################
        // #########################################################################

        gameTest.update(dt);
        gameTest.render();
        
        // reset viewport   
        glViewport(0, 0, width, height);

        // get texture from FBO into the cabinet screen BEFORE inputs from keyboard
        scene.setArcadeScreenTex(gameTest.getFBOTex());
       
        // #########################################################################
        // ########################## 3D scene rendering ###########################
        // #########################################################################

        scene.drawScene();
        scene.drawAim();

        ui.render(); //over everything

        glfwSwapBuffers(window);
       
    }
    ui.shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
