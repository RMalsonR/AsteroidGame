
# You are free to modify this file

cmake_minimum_required(VERSION 3.0)
project(game)
find_package(X11 REQUIRED)

set(CMAKE_CXX_STANDARD 11)

# for macos
include_directories(/opt/X11/include)
link_directories(/opt/X11/lib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/highScore.txt
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/level1.csv
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/level2.csv
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/level3.csv
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/level4.csv
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gameData/level5.csv
        ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
file(GLOB SRC *.cpp)
add_executable(game ${SRC})
target_link_libraries(game m X11)
