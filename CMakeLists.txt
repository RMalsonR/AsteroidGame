
# You are free to modify this file

cmake_minimum_required(VERSION 3.0)
project(game)
find_package(X11 REQUIRED)

set(CMAKE_CXX_STANDARD 11)

# for macos
include_directories(/opt/X11/include)
link_directories(/opt/X11/lib)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
file(GLOB SRC *.cpp)
add_executable(game ${SRC} SpaceObjectUtils.h SpaceObjectUtils.cpp)
target_link_libraries(game m X11)
