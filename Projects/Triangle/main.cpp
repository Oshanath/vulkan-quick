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

class Triangle : public MainLoop{
public:
    GraphicsPipeline graphicsPipeline;

    Triangle(int width, int height, std::string title):
        MainLoop(width, height, title),
        graphicsPipeline(getDevice(), getRenderPass(), getWidth(), getHeight())
    {

        Shader vertexShader("./shaders/triangle.vert.spv", getDevice());
        Shader fragmentShader("./shaders/triangle.frag.spv", getDevice());

        graphicsPipeline.setVertexShader(vertexShader);
        graphicsPipeline.setFragmentShader(fragmentShader);
        graphicsPipeline.createLayoutAndPipeline();
    }

    void render(vk::CommandBuffer& commandBuffer) override {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.graphicsPipeline);
        commandBuffer.setViewport(0, {vk::Viewport(0.0f, 0.0f, static_cast<float>(getWidth()), static_cast<float>(getHeight()), 0.0f, 1.0f)});
        commandBuffer.setScissor(0, {vk::Rect2D({0, 0}, {static_cast<uint32_t>(getWidth()), static_cast<uint32_t>(getHeight())})});
        commandBuffer.draw(3, 1, 0, 0);
    }
};

int main() {

    Triangle triangle(800, 600, "Triangle Renderer");
    triangle.run();

}