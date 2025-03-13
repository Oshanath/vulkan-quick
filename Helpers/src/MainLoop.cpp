//
// Created by User on 3/9/2025.
//

#include <utility>

#include "MainLoop.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

MainLoop::MainLoop(int width, int height, std::string title):
    Application(width, height, std::move(title))
{
    vkbSwapchain = createSwapchain();
    swapchain = vkbSwapchain.swapchain;
    renderPass = createRenderPass();
    swapchainFramebuffers = createSwapchainFramebuffers();
    commandBuffers = allocateCommandBuffers();

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    createSyncObjects();
}

void MainLoop::run() {

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Draw frame
        device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        device.resetFences(1, &inFlightFences[currentFrame]);
        vk::Result result = device.acquireNextImageKHR(swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);

        commandBuffers[currentFrame].reset();
        commandBuffers[currentFrame].begin(vk::CommandBufferBeginInfo({}));
        renderPass.beginRenderPass(swapchainFramebuffers[imageIndex], vk::Extent2D(width, height), commandBuffers[currentFrame]);

        render(commandBuffers[currentFrame], currentFrame);

        commandBuffers[currentFrame].endRenderPass();
        commandBuffers[currentFrame].end();

        vk::PipelineStageFlags pipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        graphicsQueue.submit({vk::SubmitInfo(1, &imageAvailableSemaphores[currentFrame], &pipelineStageFlags, 1, &commandBuffers[currentFrame], 1, &renderFinishedSemaphores[currentFrame])}, inFlightFences[currentFrame]);

        vk::PresentInfoKHR presentInfo(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, &imageIndex);
        result = graphicsQueue.presentKHR(presentInfo);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    device.waitIdle();

    glfwDestroyWindow(window);

    glfwTerminate();
}

vkb::Swapchain MainLoop::createSwapchain() {
    vkb::SwapchainBuilder swapchain_builder{ vkbDevice };
    auto swap_ret = swapchain_builder.build();
    if (!swap_ret) {
        throw std::runtime_error("Failed to create swapchain.");
    }
    return std::move(swap_ret.value());
}

RenderPass MainLoop::createRenderPass() {
    RenderPass renderPass(device, vk::Format(vkbSwapchain.image_format));
    renderPass.createRenderPass(device);
    return std::move(renderPass);
}

std::vector<vk::Framebuffer> MainLoop::createSwapchainFramebuffers() {
    std::vector<vk::Framebuffer> swapchainFramebuffers(vkbSwapchain.image_count);
    for (size_t i = 0; i < vkbSwapchain.image_count; i++) {
        std::vector<vk::ImageView> attachments = {
            vkbSwapchain.get_image_views().value()[i]
        };
        vk::FramebufferCreateInfo framebufferInfo({}, renderPass.renderPass, attachments.size(), attachments.data(), width, height, 1);
        swapchainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
    }
    return std::move(swapchainFramebuffers);
}

std::vector<vk::CommandBuffer> MainLoop::allocateCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT);
    return device.allocateCommandBuffers(allocInfo);
}

void MainLoop::createSyncObjects() {
    imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores.push_back(device.createSemaphore({}));
        renderFinishedSemaphores.push_back(device.createSemaphore({}));
        inFlightFences.push_back(device.createFence({vk::FenceCreateFlagBits::eSignaled}));
    }
}
