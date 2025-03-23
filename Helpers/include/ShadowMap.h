//
// Created by User on 3/23/2025.
//

#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <cstdint>
#include <Image.h>
#include <Scene.h>


class ShadowMap {

public:
    struct PushConstants {
        glm::mat4 viewProj;
    };

    uint32_t width;

    Image depthImage;
    vk::Sampler sampler;
    vk::RenderPass renderPass;
    vk::Framebuffer framebuffer;
    GraphicsPipeline graphicsPipeline;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PushConstantRange pushConstantRange;
    vk::DescriptorSet descriptorSet;

    ShadowMap() {}
    ShadowMap(Application& app, uint32_t width, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
    void beginRenderPass(vk::CommandBuffer& commandBuffer);

private:
    void prepareDescriptorSets(Application& app, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
};



#endif //SHADOWMAP_H
