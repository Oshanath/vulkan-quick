cmake_minimum_required(VERSION 3.10)

# Set the project name (change as needed)
project(RotatingTriangleWithCamera)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add executable
add_executable(RotatingTriangleWithCamera main.cpp)

# Shader compilation
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*")

set(SHADER_OUTPUT_DIR "shaders")
set(BUILD_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/RotatingTriangleWithCamera/shaders")

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

if(EXISTS "${CMAKE_SOURCE_DIR}/Resources")
    message(STATUS "Found Resources directory at: ${CMAKE_SOURCE_DIR}/Resources")

    # Attach a post-build command to copy the directory
    add_custom_command(
            TARGET RotatingTriangleWithCamera
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Resources"
            "$<TARGET_FILE_DIR:RotatingTriangleWithCamera>/Resources"
            COMMENT "Copying resource files to the output directory."
    )
else()
    message(STATUS "No Resources directory found at: ${CMAKE_SOURCE_DIR}/Resources")
endif()

add_custom_target(RotatingTriangleWithCameraCompileShaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(RotatingTriangleWithCamera RotatingTriangleWithCameraCompileShaders)

# Link Vulkan
target_link_libraries(RotatingTriangleWithCamera Vulkan::Vulkan vk-bootstrap::vk-bootstrap GPUOpen::VulkanMemoryAllocator glfw glm Helpers)
