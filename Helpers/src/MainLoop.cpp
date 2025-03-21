//
// Created by User on 3/9/2025.
//

#include <utility>

#include "MainLoop.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        camera->moveForward = true;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        camera->moveForward = false;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        camera->moveBackward = true;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        camera->moveBackward = false;
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        camera->moveLeft = true;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        camera->moveLeft = false;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        camera->moveRight = true;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        camera->moveRight = false;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        camera->moveUp = true;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        camera->moveUp = false;
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
        camera->moveDown = true;
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
        camera->moveDown = false;

    if (key == GLFW_KEY_P) {
        camera->print();
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &camera->lastX, &camera->lastY);
        camera->currentX = camera->lastX;
        camera->currentY = camera->lastY;
        camera->looking = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        camera->looking = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    if (camera->looking)
    {
        camera->lastX = camera->currentX;
        camera->lastY = camera->currentY;
        camera->currentX = xpos;
        camera->currentY = ypos;
    }
    else {
        camera->lastX = 0.0;
        camera->lastY = 0.0;
        camera->currentX = 0.0;
        camera->currentY = 0.0;
    }
}

MainLoop::MainLoop(int width, int height, std::string title):
    Application(width, height, std::move(title))
{
    vkbSwapchain = createSwapchain();
    swapchain = vkbSwapchain.swapchain;
    renderPass = createRenderPass();
    depthImage = createDepthImage(*this);
    swapchainFramebuffers = createSwapchainFramebuffers();
    commandBuffers = allocateCommandBuffers();
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    createSyncObjects();

    camera = Camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(-2.0f, -2.0f, -2.0f));
    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
}

void MainLoop::run() {

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        camera.move();

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
    RenderPass renderPass(device, vk::Format(vkbSwapchain.image_format), depthFormat);
    renderPass.createRenderPass(device);
    return std::move(renderPass);
}

std::vector<vk::Framebuffer> MainLoop::createSwapchainFramebuffers() {
    std::vector<vk::Framebuffer> swapchainFramebuffers(vkbSwapchain.image_count);
    for (size_t i = 0; i < vkbSwapchain.image_count; i++) {
        std::vector<vk::ImageView> attachments = {
            vkbSwapchain.get_image_views().value()[i],
            depthImage.imageView
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
