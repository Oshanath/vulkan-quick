//
// Created by User on 3/23/2025.
//

#include "ShadowMap.h"

#include <Scene.h>

ShadowMap::ShadowMap(Application& app, uint32_t width, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene):
    width(width)
{
    depthImage = createDepthImage(app, width, width);
    app.setNameOfObject(depthImage.image, "Image: Shadow map depth image");
    app.setNameOfObject(depthImage.imageView, "ImageView: Shadow map depth image view");

    sampler = app.device.createSampler({{}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, 0.0f, VK_TRUE, 16.0f, VK_FALSE, vk::CompareOp::eLess, 0.0f, 1.0f, vk::BorderColor::eFloatOpaqueWhite, VK_FALSE});
    app.setNameOfObject(sampler, "Sampler: Shadow map sampler");

    vk::AttachmentDescription depthAttachment = vk::AttachmentDescription({}, vk::Format::eD32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
    vk::AttachmentReference depthAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 0, VK_NULL_HANDLE, nullptr, &depthAttachmentRef, 0, nullptr);
    vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo({}, 1, &depthAttachment, 1, &subpass);
    vk::SubpassDependency dependency = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::PipelineStageFlagBits::eEarlyFragmentTests,
        {vk::AccessFlagBits::eDepthStencilAttachmentWrite},
        vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    renderPass = app.device.createRenderPass(renderPassCreateInfo);

    vk::FramebufferCreateInfo framebufferInfo({}, renderPass, 1, &depthImage.imageView, width, width, 1);
    framebuffer = app.device.createFramebuffer(framebufferInfo);

    graphicsPipeline = GraphicsPipeline(app.device, renderPass, width, width);
    prepareDescriptorSets(app, uniformBuffer, alignedUBOSize, scene);
    pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants));
    Shader vertexShader("./shaders/shadow.vert.spv", app.device);
    Shader fragmentShader("./shaders/shadow.frag.spv", app.device);
    graphicsPipeline.setVertexShader(vertexShader);
    graphicsPipeline.setFragmentShader(fragmentShader);
    graphicsPipeline.createLayoutAndPipeline(app.device, std::vector{descriptorSetLayout}, std::vector{pushConstantRange});
}

void ShadowMap::beginRenderPass(vk::CommandBuffer& commandBuffer) {
    std::array clearValues = {
        vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))
    };
    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, framebuffer, vk::Rect2D({0, 0}, vk::Extent2D{width, width}), clearValues.size(), clearValues.data());
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void ShadowMap::prepareDescriptorSets(Application& app, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene) {
    std::array bindings = {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eAll),
            vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per mesh data
            vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per instance data
        };
        descriptorSetLayout = app.device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool}, bindings.size(), bindings.data(), nullptr));
        descriptorSet = app.device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(app.descriptorPool, 1, &descriptorSetLayout))[0];

        vk::DescriptorBufferInfo bufferInfo(uniformBuffer.buffer, 0, alignedUBOSize);
        vk::DescriptorBufferInfo perMeshBufferInfo(scene.perMeshBuffer.buffer, 0, sizeof(PerMeshData) * scene.perMeshData.size());
        vk::DescriptorBufferInfo perInstanceBufferInfo(scene.perInstanceBuffer.buffer, 0, sizeof(PerInstanceData) * scene.perInstanceData.size());

        std::array descriptorWrites = {
            vk::WriteDescriptorSet(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBufferDynamic, nullptr, &bufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perMeshBufferInfo),
            vk::WriteDescriptorSet(descriptorSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perInstanceBufferInfo),
        };
        app.device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}
