cmake_minimum_required(VERSION 3.10)

# Set the project name (change as needed)
project(RotatingTriangleBindless)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add executable
add_executable(RotatingTriangleBindless main.cpp)

# Shader compilation
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*")

set(SHADER_OUTPUT_DIR "shaders")
set(BUILD_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/RotatingTriangleBindless/shaders")

# Ensure output directories exist
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
file(MAKE_DIRECTORY ${BUILD_SHADER_OUTPUT_DIR})

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_FILE_TARGET "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")
    set(SPIRV_FILE_BUILD "${BUILD_SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
            OUTPUT ${SPIRV_FILE_TARGET} ${SPIRV_FILE_BUILD}
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${SPIRV_FILE_TARGET} ${SHADER}
            COMMAND ${CMAKE_COMMAND} -E copy ${SPIRV_FILE_TARGET} ${SPIRV_FILE_BUILD}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${SHADER_NAME}"
            VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_FILE_TARGET} ${SPIRV_FILE_BUILD})
endforeach()

add_custom_target(RotatingTriangleBindlessCompileShaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(RotatingTriangleBindless RotatingTriangleBindlessCompileShaders)

# Link Vulkan
target_link_libraries(RotatingTriangleBindless Vulkan::Vulkan vk-bootstrap::vk-bootstrap GPUOpen::VulkanMemoryAllocator glfw glm Helpers)
