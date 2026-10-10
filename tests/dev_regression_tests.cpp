#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "test_support.h"
#include "glfw_input_fake.h"
#include "core/runtime_options.h"
#include "core/geometry.h"
#include "core/resource_manager.h"
#include "app/game2D/game_session.h"
#include "app/game2D/game_data.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <limits>
#include <type_traits>
namespace {
namespace fs=std::filesystem;
struct Temp {
    fs::path path=fs::temp_directory_path()/("RoyalGreen-test-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    Temp() { fs::create_directories(path); }
    ~Temp() { std::error_code error;if(path.parent_path()==fs::temp_directory_path())fs::remove_all(path,error); }
    void assets() { for(auto folder:{"shaders","meshes","textures"})fs::create_directories(path/"res"/folder); }
};
struct Encounter {
    glfw_input_fake::ScopedState fake;
    InputManager input;ActionMap actions{input};
    Encounter() {
        ResourceManager::clear();
        for(const auto& name:{gameData::playerTexName,gameData::catTexName,gameData::arrowTexName,gameData::energyBallTexName})ResourceManager::texturesMap.try_emplace(name);
        for(int key:{GLFW_KEY_A,GLFW_KEY_D,GLFW_KEY_M,GLFW_KEY_H,GLFW_KEY_S})input.registerKey(key);
        actions.bind(Action::P1MoveLeft,GLFW_KEY_A);actions.bind(Action::P1MoveRight,GLFW_KEY_D);
        actions.bind(Action::GameMenu,GLFW_KEY_M);actions.bind(Action::HelpOverlay2D,GLFW_KEY_H);actions.bind(Action::P1Attack,GLFW_KEY_S);
    }
    ~Encounter() { ResourceManager::clear(); }
    void poll(int key,int state) { glfw_input_fake::setKey(key,state);input.update(nullptr); }
};
}
TEST_CASE("Triangle snapshots survive vertex storage destruction", "[geometry][dev]") {
    std::vector<Vertex> v={Vertex({0,0,0},{0,0,1},{0,0}),Vertex({1,0,0},{0,0,1},{1,0}),Vertex({0,1,0},{0,0,1},{0,1})};
    TriangleData data(0,1,2,v);v.clear();v.shrink_to_fit();
    float t=-1;REQUIRE(mollerTrumbore(Ray({0.25f,0.25f,1},{0,0,-1}),data,t));REQUIRE(t==Catch::Approx(1));
}
TEST_CASE("Geometry rejects invalid indices and degenerate triangles", "[geometry][negative][dev]") {
    std::vector<Vertex> v={Vertex({0,0,0},{0,0,1},{0,0}),Vertex({1,0,0},{0,0,1},{1,0}),Vertex({2,0,0},{0,0,1},{0,1})};
    REQUIRE_THROWS_AS(TriangleData(0,1,3,v),std::out_of_range);
    REQUIRE_THROWS_AS(TriangleData(0,1,2,v),std::invalid_argument);
    REQUIRE_THROWS_AS(Ray({0,0,0},{0,0,0}),std::invalid_argument);
    REQUIRE_THROWS_AS(Ray({0,0,0},{0,0,std::numeric_limits<float>::infinity()}),std::invalid_argument);
}
TEST_CASE("Animation time partition preserves frames and residual time", "[animator][dev]") {
    Animator a(2,13),b(2,13);a.setAnimation(1,13,1,10);b.setAnimation(1,13,1,10);
    a.updateUV(0.35f,true);for(int i=0;i<7;++i)b.updateUV(0.05f,true);
    REQUIRE(a.getFrame()==4);REQUIRE(b.getFrame()==4);
    a.updateUV(0.05f,true);b.updateUV(0.05f,true);REQUIRE(a.getFrame()==5);REQUIRE(b.getFrame()==5);
}
TEST_CASE("Nonlooping clips preserve skipped shoot events and terminal frame", "[animator][dev]") {
    Animator a(6,13);a.setAnimation(1,8,2,25,false);a.updateUV(0.5f,true);
    REQUIRE(a.passedFrame(5));REQUIRE(a.getFrame()==8);REQUIRE(a.isAnimationFinished());
    a.updateUV(1,true);REQUIRE(a.getFrame()==8);REQUIRE_FALSE(a.passedFrame(5));
}
TEST_CASE("Animation rejects invalid sheets clips FPS and elapsed time", "[animator][negative][dev]") {
    REQUIRE_THROWS_AS(Animator(0,13),std::invalid_argument);Animator a(6,13);
    REQUIRE_THROWS_AS(a.setAnimation(0,8,1,25),std::invalid_argument);
    REQUIRE_THROWS_AS(a.setAnimation(1,14,1,25),std::invalid_argument);
    REQUIRE_THROWS_AS(a.setAnimation(1,8,7,25),std::invalid_argument);
    REQUIRE_THROWS_AS(a.setAnimation(1,8,1,0),std::invalid_argument);
    REQUIRE_THROWS_AS(a.updateUV(-1,true),std::invalid_argument);
}
TEST_CASE("Camera one-second travel is independent of frame rate", "[camera][dev]") {
    for(int fps:{30,60,120}) { Camera camera;auto start=camera.getPosition();for(int i=0;i<fps;++i)camera.moveForward(1.0f/fps);
        test_support::requireVec3(camera.getPosition(),start+camera.getFront()*camera.getKeyboardSpeed(),2e-5f); }
}
TEST_CASE("Camera setters reject invalid projections and nonfinite vectors", "[camera][negative][dev]") {
    REQUIRE_THROWS_AS(Camera({0,0,0},45,1,0.1f,20),std::invalid_argument);
    REQUIRE_THROWS_AS(Camera({2,0,0},180,1,0.1f,20),std::invalid_argument);
    Camera camera;REQUIRE_THROWS_AS(camera.setAspect(0),std::invalid_argument);REQUIRE_THROWS_AS(camera.setFront({0,0,0}),std::invalid_argument);
    REQUIRE_THROWS_AS(camera.getOrthoProj(0,600),std::invalid_argument);
    REQUIRE_THROWS_AS(camera.setKeyboardSpeed(-1),std::invalid_argument);
}
TEST_CASE("Input release fires exactly once after pressed or held", "[input][dev]") {
    glfw_input_fake::ScopedState fake;InputManager input;input.registerKey(GLFW_KEY_A);
    REQUIRE_FALSE(input.isReleased(GLFW_KEY_A));input.update(nullptr);REQUIRE_FALSE(input.isReleased(GLFW_KEY_A));
    glfw_input_fake::setKey(GLFW_KEY_A,GLFW_PRESS);input.update(nullptr);REQUIRE(input.isPressed(GLFW_KEY_A));
    input.update(nullptr);REQUIRE(input.isHeld(GLFW_KEY_A));glfw_input_fake::setKey(GLFW_KEY_A,GLFW_RELEASE);
    input.update(nullptr);REQUIRE(input.isReleased(GLFW_KEY_A));input.update(nullptr);REQUIRE_FALSE(input.isReleased(GLFW_KEY_A));
}
TEST_CASE("Runtime defaults and CLI overrides are explicit", "[runtime][dev]") {
    auto o=runtime::parseOptions({});REQUIRE(o.width==0);REQUIRE(o.height==0);REQUIRE(o.cameraSpeed==Catch::Approx(1.35));
    o=runtime::parseOptions({"--width","1280","--height","720","--camera-speed","2","--help"});
    REQUIRE(o.width==1280);REQUIRE(o.height==720);REQUIRE(o.cameraSpeed==2);REQUIRE(o.help);
}
TEST_CASE("Configuration accepts whitespace resolves relative assets and yields to CLI", "[runtime][dev]") {
    Temp tmp;auto file=tmp.path/"config.cfg";std::ofstream(file)<<"# demo\n width = 800\n height = 600\n assets = res\n camera_speed = 2\n";
    auto o=runtime::parseOptions({"--camera-speed","3","--config",file.u8string()});
    REQUIRE(o.width==800);REQUIRE(o.height==600);REQUIRE(o.assets==tmp.path/"res");REQUIRE(o.cameraSpeed==3);
}
TEST_CASE("Runtime rejects bad numeric dimensions unknown arguments and missing values", "[runtime][negative][dev]") {
    for(const std::vector<std::string>& args:std::vector<std::vector<std::string>>{{"--width","640"},{"--width","12.5","--height","480"},{"--camera-speed","nan"},{"--window-scale","0"},{"--mouse-sensitivity","-1"},{"--width"},{"--unknown","1"}})REQUIRE_THROWS(runtime::parseOptions(args));
}
TEST_CASE("Malformed missing and unknown configuration entries fail clearly", "[runtime][negative][dev]") {
    Temp tmp;auto file=tmp.path/"config.cfg";REQUIRE_THROWS(runtime::parseOptions({"--config",file.u8string()}));
    for(const auto& text:{"width\n","width=\n","unknown=3\n"}) { std::ofstream(file)<<text;REQUIRE_THROWS(runtime::parseOptions({"--config",file.u8string()})); }
}
TEST_CASE("Asset discovery follows executable location independently of working directory", "[runtime][dev]") {
    Temp tmp;tmp.assets();auto exe=tmp.path/"royalgreen.exe";
    REQUIRE(runtime::resolveAssets(exe)==fs::weakly_canonical(tmp.path/"res"));
    REQUIRE(runtime::resolveAssets(exe,tmp.path/"res")==fs::weakly_canonical(tmp.path/"res"));
    REQUIRE_THROWS(runtime::resolveAssets(exe,tmp.path/"absent"));
}
TEST_CASE("Asset discovery refuses incomplete packages", "[runtime][negative][dev]") {
    Temp tmp;fs::create_directories(tmp.path/"res/shaders");REQUIRE_THROWS(runtime::resolveAssets(tmp.path/"royalgreen.exe"));
}
TEST_CASE("Failed named resources leave maps intact without GPU calls", "[resources][dev]") {
    ResourceManager::clear();
    REQUIRE_THROWS_AS(ResourceManager::loadTexture("RoyalGreen-missing.png",TexParams(),"missing"),std::runtime_error);
    REQUIRE_FALSE(ResourceManager::texturesMap.count("missing"));
    REQUIRE_THROWS_AS(ResourceManager::loadShader("RoyalGreen-missing.vert","RoyalGreen-missing.frag","missing"),std::runtime_error);
    REQUIRE_FALSE(ResourceManager::shadersMap.count("missing"));
}
TEST_CASE("GPU owners cannot be copied and zero handles move safely", "[resources][dev]") {
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<GameSession>);
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<GameSession>);
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<Texture>);STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<Shader>);STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<Mesh>);
    Texture texture;Texture moved(std::move(texture));REQUIRE(texture.getID()==0);REQUIRE(moved.getID()==0);
    Shader shader;Shader movedShader(std::move(shader));REQUIRE(shader.getID()==0);REQUIRE(movedShader.getID()==0);
    Mesh mesh;Mesh movedMesh(std::move(mesh));REQUIRE(mesh.getVAO()==0);REQUIRE(movedMesh.getVAO()==0);
}
TEST_CASE("Empty mesh calculations fail rather than return nonfinite vectors", "[mesh][negative][dev]") {
    Mesh mesh;REQUIRE_THROWS_AS(mesh.getCenter(),std::invalid_argument);REQUIRE_THROWS_AS(mesh.getGlobalNormal(),std::invalid_argument);
}
TEST_CASE_METHOD(Encounter,"Fixed arena keeps player and camera inside initial view", "[session][dev]") {
    GameSession game(600,600);poll(GLFW_KEY_D,GLFW_PRESS);input.update(nullptr);game.input(actions,0);
    for(int i=0;i<1000;++i)game.update(0.01f);
    REQUIRE(game.cameraX()==0);REQUIRE(game.player().getPosition().x==Catch::Approx(500));REQUIRE(game.cat().getPosition().x>=0);
    // Isolate arena movement from a new AI barrage at the right-hand boundary.
    poll(GLFW_KEY_D,GLFW_RELEASE);poll(GLFW_KEY_A,GLFW_PRESS);input.update(nullptr);game.player().inputHandler(actions,0,game.projectiles());
    for(int i=0;i<1000;++i)game.update(0.01f);
    REQUIRE(game.cameraX()==0);REQUIRE(game.player().getPosition().x==Catch::Approx(0));
    auto p=game.player().getPosition();test_support::requireVec2(game.player().getBB().m_min,p+gameData::bbOffsetPosition);
}
TEST_CASE_METHOD(Encounter,"Pause resume help and reset coordinate the full CPU encounter", "[session][dev]") {
    GameSession game(600,600);poll(GLFW_KEY_D,GLFW_PRESS);input.update(nullptr);game.input(actions,0);game.update(0.1f);
    auto pos=game.player().getPosition();REQUIRE(pos.x>0);
    poll(GLFW_KEY_M,GLFW_PRESS);game.input(actions,0);REQUIRE(game.isMenuOpen());game.update(1);test_support::requireVec2(game.player().getPosition(),pos);
    game.closeMenu();poll(GLFW_KEY_M,GLFW_RELEASE);game.input(actions,0);game.update(0.1f);REQUIRE(game.player().getPosition().x>pos.x);
    poll(GLFW_KEY_H,GLFW_PRESS);game.input(actions,0);REQUIRE_FALSE(game.help());
    game.cat().hasBeenHit();game.projectiles().emit(gameData::arrowTexName,{20,100},{40,8},{600,0},true,ProjectileOwner::Player);
    game.reset();REQUIRE(game.cat().getLives()==9);REQUIRE(game.projectiles().getProjectiles().empty());REQUIRE_FALSE(game.isMenuOpen());
    test_support::requireVec2(game.player().getPosition(),gameData::playerPosition);test_support::requireVec2(game.cat().getBB().m_min,gameData::catPosition+gameData::bbOffsetPosition);
}
TEST_CASE_METHOD(Encounter,"Encounter opens its terminal menu and can start a clean rematch", "[session][dev]") {
    GameSession game(600,600);for(int i=0;i<9;++i)game.cat().hasBeenHit();
    for(int i=0;i<30;++i)game.update(0.1f);
    REQUIRE(game.cat().getState()==State::Dead);REQUIRE(game.isMenuOpen());game.reset();
    REQUIRE(game.cat().getState()==State::Idle);REQUIRE(game.cat().getLives()==9);REQUIRE_FALSE(game.isMenuOpen());
}
TEST_CASE_METHOD(Encounter,"Player shoot event survives skipped animation frames and immediate rearm", "[gameplay][dev]") {
    Player player(gameData::playerTexName,gameData::playerPosition,gameData::playerSize,{});ProjectileManager shots;
    for(int i=0;i<2;++i) { poll(GLFW_KEY_S,GLFW_PRESS);player.inputHandler(actions,0,shots);player.update(0.5f,shots);REQUIRE(player.getState()==State::Idle);poll(GLFW_KEY_S,GLFW_RELEASE); }
    REQUIRE(shots.getProjectiles().size()==2);
}
TEST_CASE_METHOD(Encounter,"Collision checks consume projectiles only once", "[collision][dev]") {
    Player player(gameData::playerTexName,gameData::playerPosition,gameData::playerSize,{});
    Cat cat(gameData::catTexName,gameData::catPosition,gameData::catSize,{});ProjectileManager shots;CollisionSystem collisions(player,cat,shots);
    shots.emit(gameData::arrowTexName,cat.getBB().m_min,{5,5},{0,0},true,ProjectileOwner::Player);
    collisions.checkCollision();REQUIRE(cat.getLives()==8);collisions.checkCollision();REQUIRE(cat.getLives()==8);
}

TEST_CASE("Imported mesh caches omit zero-area faces while retaining draw indices", "[mesh][import][dev]") {
    Mesh mesh;mesh.m_vertices={Vertex({0,0,0},{0,0,1},{0,0}),Vertex({1,0,0},{0,0,1},{1,0}),Vertex({0,1,0},{0,0,1},{0,1})};
    mesh.m_indices={0,1,2,0,0,1};mesh.rebuildTriangleCache();REQUIRE(mesh.m_indices.size()==6);REQUIRE(mesh.m_triangleData.size()==1);
    float t=-1;REQUIRE(mesh.intersectRayTriangle(Ray({0.25f,0.25f,1},{0,0,-1}),t));REQUIRE(t==Catch::Approx(1));
}
TEST_CASE("Imported mesh rejects invalid or incomplete indices", "[mesh][import][negative][dev]") {
    Mesh mesh;mesh.m_vertices={Vertex({0,0,0},{0,0,1},{0,0})};mesh.m_indices={0,0};REQUIRE_THROWS_AS(mesh.rebuildTriangleCache(),std::invalid_argument);
    mesh.m_indices={0,0,3};REQUIRE_THROWS_AS(mesh.rebuildTriangleCache(),std::out_of_range);
}
