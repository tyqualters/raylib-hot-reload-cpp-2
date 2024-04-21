cmake_minimum_required(VERSION 3.29)
project(gamelib)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_library(gamelib SHARED src/main.cpp
        ../include/lib.hpp
)

set_target_properties(gamelib PROPERTIES
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

get_target_property(TARGET_TYPE gamelib TYPE)
if (TARGET_TYPE STREQUAL SHARED_LIBRARY OR BUILD_SHARED_LIBS)
    target_compile_definitions(gamelib
            PRIVATE $<BUILD_INTERFACE:BUILD_LIBTYPE_SHARED>
            INTERFACE $<INSTALL_INTERFACE:USE_LIBTYPE_SHARED>
    )
endif()

find_package(raylib CONFIG REQUIRED)
target_link_libraries(MyGame PRIVATE raylib)