//
// Created by User on 3/9/2025.
//

#include <utility>

#include "MainLoop.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

MainLoop::MainLoop(int width, int height, std::string title):
    width(width),
    height(height),
    window(createWindow(width, height, std::move(title))),
    extensionCount(0),
    vkbInstance(createInstance()),
    surface(createSurface()),
    vkbPhysicalDevice(selectPhysicalDevice()),
    vkbDevice(createDevice()),
    device(vkbDevice.device),
    graphicsQueue(getGraphicsQueue()),
    vkbSwapchain(createSwapchain()),
    swapchain(vkbSwapchain.swapchain),
    renderPass(createRenderPass()),
    swapchainFramebuffers(createSwapchainFramebuffers()),
    commandPool(createCommandPool()),
    commandBuffers(allocateCommandBuffers()),
    imageIndex(0),
    currentFrame(0),
    vmaAllocator(createVmaAllocator())
{
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

        render(commandBuffers[currentFrame]);

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

GLFWwindow * MainLoop::createWindow(int width, int height, std::string title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

vkb::Instance MainLoop::createInstance() {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.enable_validation_layers()
        .use_default_debug_messenger()
        .build();
    if (!inst_ret) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    return std::move(inst_ret.value());
}

VkSurfaceKHR MainLoop::createSurface() {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    if (vkCreateWin32SurfaceKHR(vkbInstance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

vkb::PhysicalDevice MainLoop::selectPhysicalDevice() {
    vkb::PhysicalDeviceSelector physicalDeviceSelector({ vkbInstance });
    auto phys_ret = physicalDeviceSelector.set_surface(surface)
        .set_minimum_version(1, 1) // require a vulkan 1.1 capable device
        .require_dedicated_transfer_queue()
        .select();
    if (!phys_ret) {
        throw std::runtime_error("Failed to select Vulkan Physical Device.");
    }
    return std::move(phys_ret.value());
}

vkb::Device MainLoop::createDevice() {
    vkb::DeviceBuilder device_builder{ vkbPhysicalDevice };
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }
    return std::move(dev_ret.value());
}

vk::Queue MainLoop::getGraphicsQueue() {
    auto graphics_queue_ret = vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        throw std::runtime_error("Failed to get graphics queue.");
    }
    VkQueue graphics_queue = graphics_queue_ret.value();
    return std::move(vk::Queue(graphics_queue));
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
    renderPass.createRenderPass();
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

vk::CommandPool MainLoop::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, vkbDevice.get_queue_index(vkb::QueueType::graphics).value());
    return device.createCommandPool(poolInfo);
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

VmaAllocator MainLoop::createVmaAllocator() {

    VmaAllocator allocator;
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = vkbPhysicalDevice.physical_device;
    allocatorInfo.device = device;
    allocatorInfo.instance = vkbInstance.instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    auto result = vmaCreateAllocator(&allocatorInfo, &allocator);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create VMA allocator.");
    }
    return allocator;
}
