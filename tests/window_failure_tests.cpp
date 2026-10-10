#include <catch2/catch_test_macros.hpp>
#include "core/utilities.h"
#include <limits>
namespace { bool initSuccess=false;int createCalls=0; }
extern "C" {
int glfwInit() { return initSuccess?GLFW_TRUE:GLFW_FALSE; }
void glfwTerminate() {}
GLFWmonitor* glfwGetPrimaryMonitor() { return nullptr; }
const GLFWvidmode* glfwGetVideoMode(GLFWmonitor*) { return nullptr; }
void glfwWindowHint(int,int) {}
GLFWwindow* glfwCreateWindow(int,int,const char*,GLFWmonitor*,GLFWwindow*) { ++createCalls;return nullptr; }
void glfwMakeContextCurrent(GLFWwindow*) {}
GLFWglproc glfwGetProcAddress(const char*) { return nullptr; }
double glfwGetTime() { return 0; }
}
TEST_CASE("Window initialization stops before creation when GLFW fails", "[window][negative]") {
    initSuccess=false;createCalls=0;REQUIRE(initGLFWwindow(640,480,"test")==nullptr);REQUIRE(createCalls==0);
}
TEST_CASE("Window creation failure returns null and GLAD never owns cleanup", "[window][negative]") {
    initSuccess=true;createCalls=0;REQUIRE(initGLFWwindow(640,480,"test")==nullptr);REQUIRE(createCalls==1);REQUIRE_FALSE(initGLAD(nullptr));initSuccess=false;
}
TEST_CASE("Window validation rejects bad sizes and monitor scale before native calls", "[window][negative]") {
    REQUIRE_THROWS_AS(initGLFWwindow(0,600,"test"),std::invalid_argument);
    REQUIRE_THROWS_AS(initGLFWwindow("test",0.0f),std::runtime_error);
    REQUIRE_THROWS_AS(initGLFWwindow("test",std::numeric_limits<float>::quiet_NaN()),std::runtime_error);
}
