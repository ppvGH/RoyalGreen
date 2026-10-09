cmake_minimum_required(VERSION 3.20)
project(RoyalGreenTests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
enable_testing()
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Compile Catch itself; all other fetched dependencies supply headers only.
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR royalgreen_headers_only
)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR royalgreen_headers_only
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR royalgreen_headers_only
)
# Match the app's resolved stb revision, never a moving branch or a shallow SHA.
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
    SOURCE_SUBDIR royalgreen_headers_only
)
# An absent SOURCE_SUBDIR populates headers without building GLFW/ImGui or
# using the deprecated single-argument FetchContent_Populate interface.
FetchContent_MakeAvailable(Catch2 glm glfw imgui stb)

get_filename_component(ROYALGREEN_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

add_executable(royalgreen_tests
    core_tests.cpp
    game_camera_tests.cpp
    gameplay_tests.cpp
    mesh_tests.cpp
    glfw_input_fake.cpp
    "${ROYALGREEN_ROOT}/src/core/geometry.cpp"
    "${ROYALGREEN_ROOT}/src/core/collision.cpp"
    "${ROYALGREEN_ROOT}/src/core/input_manager.cpp"
    "${ROYALGREEN_ROOT}/src/core/action_map.cpp"
    "${ROYALGREEN_ROOT}/src/graphics/camera.cpp"
    "${ROYALGREEN_ROOT}/src/graphics/mesh.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/animator.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/game_obj.cpp"
    "${ROYALGREEN_ROOT}/src/core/resource_manager.cpp"
    "${ROYALGREEN_ROOT}/src/graphics/shader.cpp"
    "${ROYALGREEN_ROOT}/src/graphics/texture.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/sprite.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/sprite_renderer.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/player.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/cat.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/projectile.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/projectile_manager.cpp"
    "${ROYALGREEN_ROOT}/src/app/game2D/collision_system.cpp"
    "${ROYALGREEN_ROOT}/external/glad/src/glad.c"
)
target_include_directories(royalgreen_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${ROYALGREEN_ROOT}/src"
    "${ROYALGREEN_ROOT}/external/glad/include"
    "${glm_SOURCE_DIR}"
    "${stb_SOURCE_DIR}"
    "${glfw_SOURCE_DIR}/include"
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)
target_compile_definitions(royalgreen_tests PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(royalgreen_tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(royalgreen_tests DISCOVERY_TIMEOUT 30)

add_test(NAME assets.integrity
    COMMAND "${Python3_EXECUTABLE}" "${ROYALGREEN_ROOT}/scripts/verify_assets.py")
add_test(NAME sources.comments_only
    COMMAND "${Python3_EXECUTABLE}" "${ROYALGREEN_ROOT}/scripts/check_source_unchanged.py")
add_test(NAME repository.validators
    COMMAND "${Python3_EXECUTABLE}" -m unittest discover
            -s "${ROYALGREEN_ROOT}/tests" -p test_repository_checks.py)
set_tests_properties(assets.integrity sources.comments_only repository.validators
    PROPERTIES WORKING_DIRECTORY "${ROYALGREEN_ROOT}")
