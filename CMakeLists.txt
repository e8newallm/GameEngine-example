cmake_minimum_required(VERSION 3.25)
project(Game LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(gameengine)
add_subdirectory(src)
