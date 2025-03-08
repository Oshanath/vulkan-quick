cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 23)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanTemplate")

find_package(Vulkan REQUIRED)

add_subdirectory(vk-bootstrap)
add_subdirectory(glfw)
add_subdirectory(glm)

add_subdirectory(Triangle)
add_subdirectory(Helpers)
