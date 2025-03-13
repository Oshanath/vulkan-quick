//
// Created by User on 3/9/2025.
//

#ifndef MAINLOOP_H
#define MAINLOOP_H

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

#include "Shader.h"
#include "VkBootstrap.h"

class MainLoop {
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
    vkb::Swapchain vkbSwapchain;
    vk::SwapchainKHR swapchain;
    vk::Queue graphicsQueue;
    RenderPass renderPass;
    std::vector<vk::Framebuffer> swapchainFramebuffers;
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    VmaAllocator vmaAllocator;
    vk::DescriptorPool descriptorPool;

    uint32_t imageIndex = 0;
    uint32_t currentFrame = 0;

    MainLoop(int width, int height, std::string title);
    void run();
    virtual void render(vk::CommandBuffer& commandBuffer, int currentFrame) = 0;

    GLFWwindow* createWindow(int width, int height, std::string title);
    vkb::Instance createInstance();
    VkSurfaceKHR createSurface();
    vkb::PhysicalDevice selectPhysicalDevice();
    vkb::Device createDevice();
    vk::Queue getGraphicsQueue();
    vkb::Swapchain createSwapchain();
    RenderPass createRenderPass();
    std::vector<vk::Framebuffer> createSwapchainFramebuffers();
    vk::CommandPool createCommandPool();
    std::vector<vk::CommandBuffer> allocateCommandBuffers();
    void createSyncObjects();
    VmaAllocator createVmaAllocator();
    vk::DescriptorPool createDescriptorPool();

    vk::CommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
    size_t getAlignedUBOSize(size_t originalSize);
};



#endif //MAINLOOP_H
