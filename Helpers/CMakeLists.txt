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

    ${PROJECT_SOURCE_DIR}/external/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_vulkan.cpp
        src/ShadowMap.cpp
        include/ShadowMap.h
        src/VarianceShadowMap.cpp
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
        ${PROJECT_SOURCE_DIR}/external/imgui
        ${PROJECT_SOURCE_DIR}/external/imgui/backends
)