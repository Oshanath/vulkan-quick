cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 23)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanTemplate")

find_package(Vulkan REQUIRED)

add_subdirectory(external/vk-bootstrap)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/vma)
add_subdirectory(external/assimp)

add_subdirectory(Helpers)
add_subdirectory(Projects/Triangle)
add_subdirectory(Projects/TriangleWithVertexBuffers)
add_subdirectory(Projects/RotatingTriangleWithUniformBuffers)
add_subdirectory(Projects/RotatingTriangleBindless)
add_subdirectory(Projects/RotatingTriangleWithTextures)
add_subdirectory(Projects/RotatingTriangleWithDepth)
add_subdirectory(Projects/RotatingTriangleWithCamera)
add_subdirectory(Projects/TrashGod)