cmake_minimum_required(VERSION 3.10)

add_library(Helpers STATIC
    src/Shader.cpp
)

target_include_directories(Helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)