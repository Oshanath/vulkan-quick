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

class Triangle : public MainLoop{
public:
    GraphicsPipeline graphicsPipeline;
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    Buffer vertexBuffer;

    Triangle(int width, int height, std::string title):
        MainLoop(width, height, title),
        graphicsPipeline(device, renderPass, width, height),
        vertexBuffer(vmaAllocator, sizeof(vertices[0]) * vertices.size(), vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU)
    {

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
        graphicsPipeline.createLayoutAndPipeline();

        vertexBuffer.copyData(vmaAllocator, (void*)vertices.data(), sizeof(vertices[0]) * vertices.size());
    }

    void render(vk::CommandBuffer& commandBuffer) override {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.graphicsPipeline);
        commandBuffer.setViewport(0, {vk::Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f)});
        commandBuffer.setScissor(0, {vk::Rect2D({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)})});
        commandBuffer.bindVertexBuffers(0, {vertexBuffer.buffer}, {0});
        commandBuffer.draw(vertices.size(), 1, 0, 0);
    }
};

int main() {

    Triangle triangle(800, 600, "Triangle Renderer");
    triangle.run();

}