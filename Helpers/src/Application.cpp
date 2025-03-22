//
// Created by User on 3/13/2025.
//

#include "Application.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_win32.h>

Application::Application(int width, int height, std::string title):
    width(width), height(height), depthFormat(vk::Format::eD32Sfloat)
{
    window = createWindow(width, height, std::move(title));
    vkbInstance = createInstance();
    surface = createSurface();
    vkbPhysicalDevice = selectPhysicalDevice();
    vkbDevice = createDevice();
    device = vkbDevice.device;
    commandPool = createCommandPool();
    vmaAllocator = createVmaAllocator();
    descriptorPool = createDescriptorPool();
    graphicsQueue = getGraphicsQueue();
}

GLFWwindow * Application::createWindow(int width, int height, std::string title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

vkb::Instance Application::createInstance() {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.enable_validation_layers()
        .use_default_debug_messenger()
        .build();
    if (!inst_ret) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
    return std::move(inst_ret.value());
}

VkSurfaceKHR Application::createSurface() {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    if (vkCreateWin32SurfaceKHR(vkbInstance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

vkb::PhysicalDevice Application::selectPhysicalDevice() {

    vk::PhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
    descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
    descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

    vk::PhysicalDeviceFeatures features{};
    features.samplerAnisotropy = VK_TRUE;
    features.multiDrawIndirect = VK_TRUE;

    vkb::PhysicalDeviceSelector physicalDeviceSelector({ vkbInstance });
    auto phys_ret = physicalDeviceSelector.set_surface(surface)
        .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)    // needed for descriptor indexing in Vulkan 1.1
        .add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME) // only strictly needed on Vulkan 1.1
        .add_required_extension_features(descriptorIndexingFeatures)
        .set_required_features(features)
        .add_required_extension(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME)
        .add_required_extension(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME)
        .set_minimum_version(1, 1) // require a vulkan 1.1 capable device
        .require_dedicated_transfer_queue()
        .select();
    if (!phys_ret) {
        throw std::runtime_error("Failed to select Vulkan Physical Device.");
    }
    return std::move(phys_ret.value());
}

vkb::Device Application::createDevice() {

    auto device_builder = vkb::DeviceBuilder{ vkbPhysicalDevice };
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }
    return std::move(dev_ret.value());
}

vk::CommandPool Application::createCommandPool() {
    vk::CommandPoolCreateInfo poolInfo({vk::CommandPoolCreateFlagBits::eResetCommandBuffer}, vkbDevice.get_queue_index(vkb::QueueType::graphics).value());
    return device.createCommandPool(poolInfo);
}

VmaAllocator Application::createVmaAllocator() {

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

vk::DescriptorPool Application::createDescriptorPool() {
    std::vector poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eSampler, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, 1000),
        vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, 1000)
    };

    vk::DescriptorPoolCreateInfo poolInfo({vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind}, MAX_FRAMES_IN_FLIGHT, 1, poolSizes.data());
    return device.createDescriptorPool(poolInfo);
}

vk::CommandBuffer Application::beginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Application::endSingleTimeCommands(vk::CommandBuffer commandBuffer) {
    commandBuffer.end();
    graphicsQueue.submit({vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr)}, nullptr);
    graphicsQueue.waitIdle();
}

size_t Application::getAlignedUBOSize(size_t originalSize) {
    size_t minUBOAlignment = vkbPhysicalDevice.properties.limits.minUniformBufferOffsetAlignment;
    return (originalSize + minUBOAlignment - 1) & ~(minUBOAlignment - 1);
}

vk::Queue Application::getGraphicsQueue() {
    auto graphics_queue_ret = vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        throw std::runtime_error("Failed to get graphics queue.");
    }
    VkQueue graphics_queue = graphics_queue_ret.value();
    return std::move(vk::Queue(graphics_queue));
}