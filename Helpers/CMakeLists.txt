cmake_minimum_required(VERSION 3.10)

add_library(Helpers
    src/Shader.cpp
    src/GraphicsPipeline.cpp
    src/RenderPass.cpp
    src/MainLoop.cpp
    src/Buffer.cpp
    src/Image.cpp
    src/Application.cpp
    src/Camera.cpp
        src/Scene.cpp
        include/Scene.h
)

target_link_libraries(Helpers
    PUBLIC
    Vulkan::Vulkan
    glm
    vk-bootstrap
    glfw
    assimp
)
target_include_directories(Helpers PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)