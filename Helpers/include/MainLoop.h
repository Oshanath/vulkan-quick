//
// Created by User on 3/9/2025.
//

#ifndef MAINLOOP_H
#define MAINLOOP_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GraphicsPipeline.h>

#include <iostream>

#include "Application.h"
#include "VkBootstrap.h"

class MainLoop : public Application{
public:
    vkb::Swapchain vkbSwapchain;
    vk::SwapchainKHR swapchain;
    RenderPass renderPass;
    std::vector<vk::Framebuffer> swapchainFramebuffers;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;

    uint32_t imageIndex = 0;
    uint32_t currentFrame = 0;

    MainLoop(int width, int height, std::string title);
    void run();
    virtual void render(vk::CommandBuffer& commandBuffer, int currentFrame) = 0;

    vkb::Swapchain createSwapchain();
    RenderPass createRenderPass();
    std::vector<vk::Framebuffer> createSwapchainFramebuffers();
    std::vector<vk::CommandBuffer> allocateCommandBuffers();
    void createSyncObjects();
};



#endif //MAINLOOP_H
