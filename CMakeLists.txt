cmake_minimum_required(VERSION 3.24)
project(royalgreen VERSION 0.2.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(ROYALGREEN_STATIC_RUNTIME "Embed MSVC runtime for portable Windows packages" ON)
if(MSVC AND ROYALGREEN_STATIC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
include(cmake/dependencies.cmake)
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

# ------------------------------------------------------------------------------
# @?!@?!@?!@?!@?!############### TARGET DEFINITION ##############!?@!?@!?@!?@!?@
# ------------------------------------------------------------------------------

# Exe --------------------------------------------------------------------------
add_executable(royalgreen
  src/main.cpp
  src/core/runtime_options.cpp
  src/core/graphics_checks.cpp
  src/app/game2D/game_session.cpp

  src/core/callbacks.cpp
  src/core/utilities.cpp
  src/core/resource_manager.cpp
  src/core/input_manager.cpp
  src/core/action_map.cpp
  src/core/geometry.cpp
  src/core/collision.cpp

  src/graphics/shader.cpp
  src/graphics/texture.cpp
  src/graphics/framebuffer.cpp

  src/graphics/camera.cpp

  src/graphics/mesh.cpp 
  src/graphics/material.cpp
  src/graphics/model.cpp

  src/app/ui_manager.cpp

  src/app/scene3D/scene.cpp
  src/app/scene3D/arcade.cpp
  src/app/scene3D/room.cpp

  src/app/game2D/sprite_renderer.cpp
  src/app/game2D/game.cpp
  src/app/game2D/game_obj.cpp
  src/app/game2D/sprite.cpp
  src/app/game2D/animator.cpp
  src/app/game2D/background.cpp
  src/app/game2D/player.cpp
  src/app/game2D/projectile.cpp
  src/app/game2D/projectile_manager.cpp
  src/app/game2D/cat.cpp
  src/app/game2D/collision_system.cpp
  src/app/game2D/gameHUD.cpp



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

# Development binaries get their own asset copy; no source paths are embedded.
add_custom_command(TARGET royalgreen POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:royalgreen>/res")
install(TARGETS royalgreen RUNTIME DESTINATION .)
install(DIRECTORY res DESTINATION .)
install(FILES docs/release-readme.md docs/asset-provenance.md royalgreen.cfg.example DESTINATION .)
install(FILES "${glfw_SOURCE_DIR}/LICENSE.md" DESTINATION third-party RENAME GLFW-LICENSE.md)
install(FILES "${imgui_SOURCE_DIR}/LICENSE.txt" DESTINATION third-party RENAME IMGUI-LICENSE.txt)
install(FILES "${assimp_SOURCE_DIR}/LICENSE" DESTINATION third-party RENAME ASSIMP-LICENSE.txt)
install(FILES "${stb_SOURCE_DIR}/LICENSE" DESTINATION third-party RENAME STB-LICENSE.txt)
install(FILES "${glm_SOURCE_DIR}/copying.txt" DESTINATION third-party RENAME GLM-LICENSE.txt)
install(FILES external/glad/include/glad/glad.h DESTINATION third-party RENAME GLAD-notice.h)
install(FILES external/glad/include/KHR/khrplatform.h DESTINATION third-party RENAME KHR-notice.h)
install(FILES "${assimp_SOURCE_DIR}/contrib/zlib/LICENSE" DESTINATION third-party RENAME ZLIB-LICENSE.txt)
set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME RoyalGreen)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "RoyalGreen-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
