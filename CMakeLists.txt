cmake_minimum_required(VERSION 3.15)
project(royalgreen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiled directories (they have their own CMakeLists.txt)
add_subdirectory(external/glfw)
add_subdirectory(external/assimp)

# Create ImGui library (static library)
add_library(imgui
	external/imgui/imgui.cpp
	external/imgui/imgui_demo.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/backends/imgui_impl_glfw.cpp
	external/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
	external/imgui
	external/imgui/backends
)
target_link_libraries(imgui PUBLIC glfw)

# Create glad library
add_library(glad 
	external/glad/src/glad.c
)
target_include_directories(glad PUBLIC external/glad/include)

# Executable
add_executable(royalgreen
	src/main.cpp
	src/core/callbacks.cpp
)

# External directories (not needed for compiled directories)
target_include_directories(royalgreen 
	PRIVATE
	${CMAKE_SOURCE_DIR}/external/glm
	${CMAKE_SOURCE_DIR}/external/stb

	${CMAKE_SOURCE_DIR}/src
)


# Linking
find_package(OpenGL REQUIRED)
target_link_libraries(royalgreen
	PRIVATE
	assimp
	glad
	glfw
	imgui
	OpenGL::GL
)

