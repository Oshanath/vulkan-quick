cmake_minimum_required (VERSION 3.8)

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


add_executable (VulkanTemplate "VulkanTemplate.cpp" "VulkanTemplate.h")

target_link_libraries(VulkanTemplate Vulkan::Vulkan vk-bootstrap::vk-bootstrap glfw glm)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanTemplate PROPERTY CXX_STANDARD 23)
endif()
