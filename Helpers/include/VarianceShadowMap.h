//
// Created by User on 3/23/2025.
//

#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <cstdint>
#include <Image.h>
#include <Scene.h>

enum class SATOperation : uint32_t {
    HORIZONTAL = 0,
    VERTICAL = 1
};

struct SATPushConstants {
    SATOperation operation;
};

class VarianceShadowMap {

public:

    uint32_t width;

    vk::Image momentImages;
    vk::ImageView momentImageView;
    VmaAllocation momentImageAllocation;
    Image depthImage;
    Buffer doubleMomentsBuffer;
    vk::RenderPass renderPass;
    GraphicsPipeline graphicsPipeline;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
    vk::Framebuffer framebuffer;

    vk::Pipeline satComputePipeline;
    vk::PipelineLayout satComputePipelineLayout;
    vk::Pipeline satPopulationComputePipeline;
    vk::PipelineLayout satPopulationComputePipelineLayout;


    VarianceShadowMap() {}
    VarianceShadowMap(Application& app, uint32_t width, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
    void beginRenderPass(vk::CommandBuffer& commandBuffer);

private:
    void prepareDescriptorSets(Application& app, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
    void createResources(Application& app, uint32_t width);
};



#endif //SHADOWMAP_H
