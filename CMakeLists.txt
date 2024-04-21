cmake_minimum_required(VERSION 3.29)
project(MyGame)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_executable(MyGame src/game.cpp
        src/main.cpp
        src/hotreloadlibrary.hpp
        Game/src/main.cpp
)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(MyGame PRIVATE raylib)

add_subdirectory(Game)