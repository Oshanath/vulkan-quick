//
// Created by User on 3/13/2025.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GraphicsPipeline.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include <vma/vk_mem_alloc.h>

#include "VkBootstrap.h"

class Application {
public:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    GLFWwindow* window;
    int width;
    int height;
    uint32_t extensionCount = 0;

    vkb::Instance vkbInstance;
    VkSurfaceKHR surface;
    vkb::PhysicalDevice vkbPhysicalDevice;
    vkb::Device vkbDevice;
    vk::Device device;
    vk::CommandPool commandPool;
    VmaAllocator vmaAllocator;
    vk::DescriptorPool descriptorPool;
    vk::Queue graphicsQueue;

    Application(int width, int height, std::string title);

    GLFWwindow *createWindow(int width, int height, std::string title);

    vkb::Instance createInstance();

    VkSurfaceKHR createSurface();

    vkb::PhysicalDevice selectPhysicalDevice();

    vkb::Device createDevice();

    vk::CommandPool createCommandPool();

    VmaAllocator createVmaAllocator();

    vk::DescriptorPool createDescriptorPool();

    vk::CommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

    size_t getAlignedUBOSize(size_t originalSize);

    vk::Queue getGraphicsQueue();
};



#endif //APPLICATION_H
