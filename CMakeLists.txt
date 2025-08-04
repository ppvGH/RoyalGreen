cmake_minimum_required(VERSION 3.15)
project(royalgreen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW ------------------------------------------------------------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# ImGui -----------------------------------------------------------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.9
)
FetchContent_MakeAvailable(imgui)

# Assimp ----------------------------------------------------------------------
FetchContent_Declare(
  assimp
  URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.2.tar.gz
)

# Lighter build without tests / samples / tools
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)         
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)      
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE) 
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)            

# Importers
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

# Exporters
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_EXPORTER OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# stb (header-only) ------------------------------------------------------------
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# GLM (header-only) ------------------------------------------------------------
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# OpenGL -----------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# Create ImGui lib -------------------------------------------------------------
add_library(imgui_lib
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC glfw)

# Exe --------------------------------------------------------------------------
add_executable(royalgreen
  src/main.cpp
  src/core/callbacks.cpp
  external/glad/src/glad.c
)

# Include dirs -----------------------------------------------------------------
target_include_directories(royalgreen PRIVATE
  src
  ${stb_SOURCE_DIR}
  external/glad/include
  ${glm_SOURCE_DIR}
)

# Link -------------------------------------------------------------------------
target_link_libraries(royalgreen PRIVATE
  glfw
  assimp
  imgui_lib
  OpenGL::GL
)
