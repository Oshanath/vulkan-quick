//
// Created by User on 3/23/2025.
//

#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <cstdint>
#include <Image.h>
#include <Scene.h>


class VarianceShadowMap {

public:

    uint32_t width;

    vk::Image momentImages;
    std::array<vk::ImageView, 2> individualImageViews;
    vk::ImageView combinedImageView;
    VmaAllocation momentImageAllocation;
    Image depthImage;
    vk::RenderPass renderPass;
    GraphicsPipeline graphicsPipeline;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
    vk::Framebuffer framebuffer;

    VarianceShadowMap() {}
    VarianceShadowMap(Application& app, uint32_t width, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
    void beginRenderPass(vk::CommandBuffer& commandBuffer);

private:
    void prepareDescriptorSets(Application& app, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene);
    void createMomentImages(Application& app, uint32_t width);
};



#endif //SHADOWMAP_H
