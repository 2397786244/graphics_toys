cmake_minimum_required(VERSION 3.10)
project(opengl_test)

set(CMAKE_CXX_STANDARD 11)
include_directories(./include ./include/imgui)
link_directories("./lib")
set(toy_class Camera.cpp Shader.cpp
add_executable(opengl_test glad.c ${toy_class} grass_animation.cpp)
target_link_libraries(opengl_test libglfw3.a)
