cmake_minimum_required(VERSION 3.25)
project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(gameengine)
add_subdirectory(src)
#add_subdirectory(test)