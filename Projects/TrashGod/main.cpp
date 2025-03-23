#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GraphicsPipeline.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <Image.h>

#include "Shader.h"
#include "VkBootstrap.h"
#include "MainLoop.h"

#include "Buffer.h"
#include "Scene.h"
#include "vma/vk_mem_alloc.h"

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 cameraPos;
};

struct PushConstants {
    float ambientFactor;
};

class Triangle : public MainLoop{
public:
    size_t alignedUBOSize = getAlignedUBOSize(sizeof(UniformBufferObject));
    size_t uniformBufferSize = alignedUBOSize * MAX_FRAMES_IN_FLIGHT;

    GraphicsPipeline graphicsPipeline = GraphicsPipeline(device, renderPass, width, height);
    PushConstants pushConstants {.ambientFactor = 0.09f};
    Scene scene;
    std::shared_ptr<Model> trashGod;
    Buffer uniformBuffer;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;

    Triangle(int width, int height, std::string title):
        MainLoop(width, height, title),
        uniformBuffer(*this, uniformBufferSize, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU)
    {
        trashGod = scene.addModel(*this, "Resources/trashGod/scene.fbx", 1.0f, glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        scene.lightSources.push_back(LightSource(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(1690.77f, 4482.58f, 6788.77f), glm::vec3(-0.703004, -0.437229, -0.560906), LightSourceType::DIRECTIONAL_LIGHT));
        scene.generateBuffers(*this);

        prepareDescriptorSets();
        vk::PushConstantRange pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants));
        Shader vertexShader("./shaders/triangle.vert.spv", device);
        Shader fragmentShader("./shaders/triangle.frag.spv", device);
        graphicsPipeline.setVertexShader(vertexShader);
        graphicsPipeline.setFragmentShader(fragmentShader);
        graphicsPipeline.createLayoutAndPipeline(device, std::vector{descriptorSetLayout}, std::vector{pushConstantRange});
    }

    void render(vk::CommandBuffer& commandBuffer, int currentFrame) override {
        UniformBufferObject ubo{
            .view = camera.getViewMatrix(),
            .proj = glm::perspective(glm::radians(45.0f), width / (float) height, 10.0f, 100000.0f),
            .cameraPos = glm::vec4(camera.position, 1.0f)
        };
        ubo.proj[1][1] *= -1;
        uniformBuffer.copyData(vmaAllocator, &ubo, sizeof(ubo), currentFrame * alignedUBOSize);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.graphicsPipeline);
        uint32_t dynamicOffset = currentFrame * static_cast<uint32_t>(alignedUBOSize);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline.pipelineLayout, 0, 1, &descriptorSet, 1, &dynamicOffset);
        commandBuffer.pushConstants<PushConstants>(graphicsPipeline.pipelineLayout, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, pushConstants);
        commandBuffer.setViewport(0, {vk::Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f)});
        commandBuffer.setScissor(0, {vk::Rect2D({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)})});
        commandBuffer.bindVertexBuffers(0, {scene.vertexBuffer.buffer}, {0});
        commandBuffer.bindIndexBuffer(scene.indexBuffer.buffer, 0, vk::IndexType::eUint32);
        commandBuffer.drawIndexedIndirect(scene.indirectCommandsBuffer.buffer, 0, scene.meshCount, sizeof(vk::DrawIndexedIndirectCommand));
    }

    void renderUI() override {
        ImGui::Text("Controls\n");
        ImGui::SliderFloat("Ambient Factor",  &pushConstants.ambientFactor, 0.0f, 0.25f);
    }

    void prepareDescriptorSets() {
        std::array bindings = {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eAll),
            vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per mesh data
            vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per instance data
            vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment), // Materials
            vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment) // Light sources
        };
        descriptorSetLayout = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool}, bindings.size(), bindings.data(), nullptr));
        descriptorSet = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptorPool, 1, &descriptorSetLayout))[0];

        vk::DescriptorBufferInfo bufferInfo(uniformBuffer.buffer, 0, alignedUBOSize);
        vk::DescriptorBufferInfo perMeshBufferInfo(scene.perMeshBuffer.buffer, 0, sizeof(PerMeshData) * scene.perMeshData.size());
        vk::DescriptorBufferInfo perInstanceBufferInfo(scene.perInstanceBuffer.buffer, 0, sizeof(PerInstanceData) * scene.perInstanceData.size());
        vk::DescriptorBufferInfo materialBufferInfo(scene.materialBuffer.buffer, 0, sizeof(Material) * scene.materials.size());
        vk::DescriptorBufferInfo lightSourcesBufferInfo(scene.lightSourcesBuffer.buffer, 0, sizeof(LightSource) * scene.lightSources.size());

        std::array descriptorWrites = {
            vk::WriteDescriptorSet(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBufferDynamic, nullptr, &bufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perMeshBufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perInstanceBufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 3, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &materialBufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 4, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &lightSourcesBufferInfo)
        };
        device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
};

int main() {

    Triangle triangle(1920, 1080, "Triangle Renderer");
    triangle.run();

}