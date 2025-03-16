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
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>

#include "Shader.h"
#include "VkBootstrap.h"
#include "MainLoop.h"

#include "Buffer.h"
#include "vma/vk_mem_alloc.h"

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        return {
            vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
            vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color))
        };
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Triangle : public MainLoop{
public:
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    GraphicsPipeline graphicsPipeline = GraphicsPipeline(device, renderPass, width, height);
    Buffer vertexBuffer = createBufferWithData(*this, sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_GPU_ONLY, (void*)vertices.data());
    Buffer indexBuffer = createBufferWithData(*this, sizeof(indices[0]) * indices.size(), vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_GPU_ONLY, (void*)indices.data());

    size_t alignedUBOSize = getAlignedUBOSize(sizeof(UniformBufferObject));
    size_t uniformBufferSize = alignedUBOSize * MAX_FRAMES_IN_FLIGHT;
    Buffer uniformBuffer;

    vk::DescriptorSetLayoutBinding uboLayoutBinding = vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eAll);
    vk::DescriptorBindingFlags bindingFlags = vk::DescriptorBindingFlags{
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
        // | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
    };
    vk::DescriptorSetLayoutBindingFlagsCreateInfo descriptorSetLayoutBindingFlagsCreateInfo{1, &bindingFlags};
    vk::DescriptorSetLayout descriptorSetLayout = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool}, 1, &uboLayoutBinding, &descriptorSetLayoutBindingFlagsCreateInfo));
    vk::DescriptorSet descriptorSet = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptorPool, 1, &descriptorSetLayout))[0];

    Triangle(int width, int height, std::string title):
        MainLoop(width, height, title),
        uniformBuffer(*this, uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU)
    {
        vk::DescriptorBufferInfo bufferInfo(uniformBuffer.buffer, 0, alignedUBOSize);
        vk::WriteDescriptorSet descriptorWrite(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBufferDynamic, nullptr, &bufferInfo);
        device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

        Shader vertexShader("./shaders/triangle.vert.spv", device);
        Shader fragmentShader("./shaders/triangle.frag.spv", device);

        graphicsPipeline.setVertexShader(vertexShader);
        graphicsPipeline.setFragmentShader(fragmentShader);

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        graphicsPipeline.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        graphicsPipeline.vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
        graphicsPipeline.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        graphicsPipeline.vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        graphicsPipeline.pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo({}, 1, &descriptorSetLayout, 0, nullptr);
        graphicsPipeline.createLayoutAndPipeline();
    }

    void render(vk::CommandBuffer& commandBuffer, int currentFrame) override {

        // Update Uniform Buffer
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{
            .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            .view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            .proj = glm::perspective(glm::radians(45.0f), width / (float) height, 0.1f, 10.0f)
        };
        ubo.proj[1][1] *= -1;
        uniformBuffer.copyData(vmaAllocator, &ubo, sizeof(ubo), currentFrame * alignedUBOSize);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.graphicsPipeline);
        uint32_t dynamicOffset = currentFrame * static_cast<uint32_t>(alignedUBOSize);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline.pipelineLayout, 0, 1, &descriptorSet, 1, &dynamicOffset);
        commandBuffer.setViewport(0, {vk::Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f)});
        commandBuffer.setScissor(0, {vk::Rect2D({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)})});
        commandBuffer.bindVertexBuffers(0, {vertexBuffer.buffer}, {0});
        commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint16);
        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }
};

int main() {

    Triangle triangle(1920, 1080, "Triangle Renderer");
    triangle.run();

}