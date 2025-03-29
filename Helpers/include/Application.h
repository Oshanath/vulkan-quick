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
    vk::Format depthFormat;

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

    void setNameOfObject(vk::Buffer buffer, std::string name) {
        setNameOfObject(buffer.objectType, (uint64_t)(VkBuffer)buffer, name.c_str());
    }

    void setNameOfObject(vk::Image image, std::string name) {
        setNameOfObject(image.objectType, (uint64_t)(VkImage)image, name.c_str());
    }

    void setNameOfObject(vk::ImageView image, std::string name) {
        setNameOfObject(image.objectType, (uint64_t)(VkImageView)image, name.c_str());
    }

    void setNameOfObject(vk::Pipeline pipeline, std::string name) {
        setNameOfObject(pipeline.objectType, (uint64_t)(VkPipeline)pipeline, name.c_str());
    }

    void setNameOfObject(vk::Sampler sampler, std::string name) {
        setNameOfObject(sampler.objectType, (uint64_t)(VkSampler)sampler, name.c_str());
    }

    void setNameOfObject(vk::RenderPass sampler, std::string name) {
        setNameOfObject(sampler.objectType, (uint64_t)(VkRenderPass)sampler, name.c_str());
    }

    void setNameOfObject(vk::ObjectType type, uint64_t objectHandle, std::string name)
    {
        auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(vkbInstance.instance, "vkSetDebugUtilsObjectNameEXT");

        VkDebugUtilsObjectNameInfoEXT nameInfo{};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = VkObjectType(type);
        nameInfo.objectHandle = objectHandle;
        nameInfo.pObjectName = name.c_str();

        func(device, &nameInfo);
    }
};

#endif //APPLICATION_H
