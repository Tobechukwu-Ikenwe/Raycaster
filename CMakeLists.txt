cmake_minimum_required(VERSION 3.16)
project(raycaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)

add_executable(raycaster
  src/main.cpp
  src/renderer_gl.cpp
  src/map.cpp
  src/gl_core.cpp
  src/raycaster.cpp
)

target_include_directories(raycaster PRIVATE include)
target_link_libraries(raycaster PRIVATE SDL2::SDL2 opengl32)
