#include <iostream>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include "core/callbacks.h"
#include "core/utilities.h"
#include "core/runtime_options.h"
#include "core/graphics_checks.h"
#include "app/scene3D/scene.h"
#include "app/scene3D/scene_data.h"
#include "app/ui_manager.h"
#include "app/path.h"
#include "app/game2D/game_data.h"
#ifdef _WIN32
// Driver-selection hints; Intel, AMD and NVIDIA OpenGL drivers are supported.
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement=1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance=1;
}
#endif
void assetLoader()
{
    // shaders
    ResourceManager::loadShader(Path::pathBlinnPhongVert, Path::pathBlinnPhongFrag, sceneData::blinnPhongShaderName);   // for 3D stuff
    ResourceManager::loadShader(Path::pathBlinnPhongVert, Path::pathCRTFrag, sceneData::CRTShaderName);  // for the display
    ResourceManager::loadShader(Path::pathAimVert, Path::pathAimFrag, sceneData::aimShaderName); // for the aim
    ResourceManager::loadShader(Path::pathGameVert, Path::pathGameFrag, sceneData::gameShaderName); // for the 2D game

    /* Point light depth pass. */
    ResourceManager::loadShader(Path::pathPointDepthVert, 
                                Path::pathPointDepthGeom, 
                                Path::pathPointDepthFrag, 
                                sceneData::pointLightDepthShaderName);

    /* Spot light depth pass. */
    ResourceManager::loadShader(Path::pathSpotDepthVert,
                                Path::pathSpotDepthFrag,
                                sceneData::spotLightDepthShaderName);

    // texture for 3D scene
    ResourceManager::loadTexture(Path::pathRospi, TexParams(), "rospi");
    ResourceManager::loadTexture(Path::pathFloorTile, TexParams(), "floorTile");

    // texture for 2D game
    ResourceManager::loadTexture(Path::pathBackground, TexParams(), gameData::backgroundTexName);
    ResourceManager::loadTexture(Path::pathPlayerSprite, TexParams(), gameData::playerTexName);
    ResourceManager::loadTexture(Path::pathArrow, TexParams(), gameData::arrowTexName);
    ResourceManager::loadTexture(Path::pathCatSprite, TexParams(), gameData::catTexName);
    ResourceManager::loadTexture(Path::pathEnergyBall, TexParams(), gameData::energyBallTexName);
    ResourceManager::loadTexture(Path::pathCatLife, TexParams(), gameData::catLifeTexName);

}

void keyRegistering(InputManager& input)
{
    input.registerKey(GLFW_KEY_W);
    input.registerKey(GLFW_KEY_A);
    input.registerKey(GLFW_KEY_S);
    input.registerKey(GLFW_KEY_D);

    input.registerKey(GLFW_KEY_H);
    input.registerKey(GLFW_KEY_K);      
    input.registerKey(GLFW_KEY_M);
    input.registerKey(GLFW_KEY_F);
    input.registerKey(GLFW_KEY_T);
    input.registerKey(GLFW_MOUSE_BUTTON_LEFT);

}


void keyBindings3D(ActionMap& actionMap)
{
    actionMap.bind(Action::MoveForward, GLFW_KEY_W);
    actionMap.bind(Action::MoveLeft, GLFW_KEY_A);
    actionMap.bind(Action::MoveBackward, GLFW_KEY_S);
    actionMap.bind(Action::MoveRight, GLFW_KEY_D);

    //actionMap.bind(Action::StartAnimation, GLFW_KEY_K);     // for testing
    //actionMap.bind(Action::SwitchScreen, GLFW_KEY_H);       // for testing
    actionMap.bind(Action::SwitchCameraGrounded, GLFW_KEY_F);
    actionMap.bind(Action::HelpOverlay3D, GLFW_KEY_H);
    actionMap.bind(Action::ToggleAim, GLFW_KEY_T);
    actionMap.bind(Action::SelectObject, GLFW_MOUSE_BUTTON_LEFT);
}

void keyBindings2D(ActionMap& actionMap)
{
    actionMap.bind(Action::P1Jump, GLFW_KEY_W);
    actionMap.bind(Action::P1MoveLeft, GLFW_KEY_A);
    actionMap.bind(Action::P1Attack, GLFW_KEY_S);
    actionMap.bind(Action::P1MoveRight, GLFW_KEY_D);

    actionMap.bind(Action::GameMenu, GLFW_KEY_M);
    actionMap.bind(Action::HelpOverlay2D, GLFW_KEY_H);
}



namespace {
struct GlfwCleanup { ~GlfwCleanup() { glfwTerminate(); } };
struct ResourceCleanup { ~ResourceCleanup() { ResourceManager::clear(); } };
}
int main(int argc,char** argv) {
    try {
        std::vector<std::string> arguments(argv+1,argv+argc);
        const auto options=runtime::parseOptions(arguments);
        if(options.help) { std::cout<<runtime::usage();return 0; }
        Path::configure(runtime::resolveAssets(runtime::executablePath(),options.assets));
        GlfwCleanup glfwCleanup;
        std::unique_ptr<GLFWwindow,decltype(&glfwDestroyWindow)> window(
            options.width?initGLFWwindow(options.width,options.height,"RoyalGreen"):initGLFWwindow("RoyalGreen",options.windowScale),glfwDestroyWindow);
        if(!window)throw std::runtime_error("Cannot create an OpenGL 3.3 window; check the graphics driver and desktop session");
        if(!initGLAD(window.get()))throw std::runtime_error("Cannot initialize OpenGL functions");
        ResourceCleanup resources;
        glfwSetWindowPos(window.get(),400,200);glfwSetInputMode(window.get(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        UIManager ui;ui.init(window.get());
        int width=0,height=0;glfwGetFramebufferSize(window.get(),&width,&height);
        if(width<=0 || height<=0)throw std::runtime_error("Initial framebuffer dimensions are invalid");
        assetLoader();
        InputManager input;keyRegistering(input);
        ActionMap actions3D(input),actions2D(input);keyBindings3D(actions3D);keyBindings2D(actions2D);
        Scene scene(width,height);scene.getCam3D().setKeyboardSpeed(options.cameraSpeed);scene.getCam3D().setMouseSpeed(options.mouseSensitivity);
        scene.depthPass();Callbacks::initCallbacks(window.get());
        Game game(600,600);
        std::cout<<"Rendered with: "<<glGetString(GL_RENDERER)<<std::endl;
        if(options.graphicsCheck) { game.render();glViewport(0,0,width,height);scene.setArcadeScreenTex(game.getFBOTex());scene.drawScene();runGraphicsChecks();std::cout<<"PASS: graphics/resource/lifetime checks"<<std::endl;return 0; }
        float lastFrame=static_cast<float>(glfwGetTime());
        const double started=glfwGetTime();
        while(!glfwWindowShouldClose(window.get())) {
            glfwPollEvents();
            float dt=std::min(getDeltaTime(lastFrame),0.25f);
            glfwGetFramebufferSize(window.get(),&width,&height);
            if(width<=0 || height<=0) { glfwWaitEventsTimeout(0.05);continue; }
            scene.resize(width,height);
            ResourceManager::getShader(sceneData::CRTShaderName).use().setVector3f("viewportResolution",glm::vec3(width,height,static_cast<float>(width)/height));
            input.update(window.get());
            if(ui.isMenuOpen())glfwSetInputMode(window.get(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
            else {
                glfwSetInputMode(window.get(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
                if(scene.isInput3DEnabled())scene.input3DHandler(window.get(),actions3D,dt);
                else if(scene.isInput2DEnabled())game.input2DHandler(actions2D,dt);
            }
            int logicalWidth=0,logicalHeight=0;glfwGetWindowSize(window.get(),&logicalWidth,&logicalHeight);
            ui.beginFrame();ui.drawUI(scene,game,logicalWidth,logicalHeight);
            if(scene.isDisplayOn()) {
                if(!ui.isMenuOpen() && scene.isInput2DEnabled())game.update(dt);
                game.render();
            }
            glViewport(0,0,width,height);scene.setArcadeScreenTex(game.getFBOTex());
            scene.update();scene.drawScene();ui.render();glfwSwapBuffers(window.get());
            if(options.exitAfter>0 && glfwGetTime()-started>=options.exitAfter)glfwSetWindowShouldClose(window.get(),true);
        }
        return 0;
    } catch(const std::exception& error) { std::cerr<<"RoyalGreen: "<<error.what()<<std::endl;return 1; }
}
