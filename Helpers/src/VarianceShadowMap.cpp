//
// Created by User on 3/23/2025.
//

#include "VarianceShadowMap.h"

#include <Scene.h>

VarianceShadowMap::VarianceShadowMap(Application& app, uint32_t width, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene):
    width(width)
{
    createResources(app, width);
    depthImage = createDepthImage(app, width, width);
    app.setNameOfObject(depthImage.image, "Image: Variance Shadow map depth image");

    std::array attachments = {
        vk::AttachmentDescription({}, vk::Format::eR32G32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral),
        vk::AttachmentDescription({}, vk::Format::eD32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal)
    };
    std::array colorAttachmentRefs = { vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal) };
    vk::AttachmentReference depthAttachmentRef = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    vk::SubpassDescription subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, colorAttachmentRefs.size(), colorAttachmentRefs.data(), nullptr, &depthAttachmentRef, 0, nullptr);
    vk::RenderPassCreateInfo renderPassCreateInfo = vk::RenderPassCreateInfo({}, attachments.size(), attachments.data(), 1, &subpass);
    std::array subpassDependencies = {
        vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput,
            {},
            vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite),
        vk::SubpassDependency(0, VK_SUBPASS_EXTERNAL,
            vk::PipelineStageFlagBits::eLateFragmentTests | vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eComputeShader | vk::PipelineStageFlagBits::eTopOfPipe,
            vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite,
            vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite)
    };
    renderPassCreateInfo.dependencyCount = subpassDependencies.size();
    renderPassCreateInfo.pDependencies = subpassDependencies.data();
    renderPass = app.device.createRenderPass(renderPassCreateInfo);
    app.setNameOfObject(renderPass, "RenderPass: Variance shadow map render pass");

    std::array frameBufferImageViews = {momentImageView, depthImage.imageView};
    vk::FramebufferCreateInfo framebufferInfo({}, renderPass, frameBufferImageViews.size(), frameBufferImageViews.data(), width, width, 1);
    framebuffer = app.device.createFramebuffer(framebufferInfo);

    graphicsPipeline = GraphicsPipeline(app.device, renderPass, width, width);
    prepareDescriptorSets(app, uniformBuffer, alignedUBOSize, scene);
    Shader vertexShader("./shaders/varianceShadow.vert.spv", app.device);
    Shader fragmentShader("./shaders/varianceShadow.frag.spv", app.device);
    graphicsPipeline.setVertexShader(vertexShader);
    graphicsPipeline.setFragmentShader(fragmentShader);
    graphicsPipeline.createLayoutAndPipeline(app.device, std::vector{descriptorSetLayout}, {}, renderPass);

    Shader computeShader("./shaders/sat.comp.spv", app.device);
    vk::PushConstantRange pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eCompute, 0, sizeof(SATPushConstants));
    vk::PipelineShaderStageCreateInfo computeShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, computeShader.shaderModule, "main");
    satComputePipelineLayout = app.device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &descriptorSetLayout, 1, &pushConstantRange));
    satComputePipeline = app.device.createComputePipeline(nullptr, vk::ComputePipelineCreateInfo({}, computeShaderStageCreateInfo, satComputePipelineLayout, nullptr, -1)).value;

    Shader populationComputeShader("./shaders/satPopulation.comp.spv", app.device);
    vk::PipelineShaderStageCreateInfo computeShaderStageCreateInfoPopulation({}, vk::ShaderStageFlagBits::eCompute, populationComputeShader.shaderModule, "main");
    satPopulationComputePipelineLayout = app.device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &descriptorSetLayout, 0, nullptr));
    satPopulationComputePipeline = app.device.createComputePipeline(nullptr, vk::ComputePipelineCreateInfo({}, computeShaderStageCreateInfoPopulation, satPopulationComputePipelineLayout, nullptr, -1)).value;
}

void VarianceShadowMap::beginRenderPass(vk::CommandBuffer& commandBuffer) {
    std::array clearValues = {
        vk::ClearValue(vk::ClearColorValue({ 1.0f, 1.0f, 0.0f, 1.0f })),
        vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))
    };
    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, framebuffer, vk::Rect2D({0, 0}, vk::Extent2D{width, width}), clearValues.size(), clearValues.data());
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void VarianceShadowMap::prepareDescriptorSets(Application& app, Buffer& uniformBuffer, size_t alignedUBOSize, Scene& scene) {
    std::array bindings = {
        vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eAll),
        vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per mesh data
        vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex), // Per instance data
        vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eFragment), // Light sources
        vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eCompute), // Shadow map moments sat generation
        vk::DescriptorSetLayoutBinding(5, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute) // Shadow map moments
    };
    descriptorSetLayout = app.device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool}, bindings.size(), bindings.data(), nullptr));
    descriptorSet = app.device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(app.descriptorPool, 1, &descriptorSetLayout))[0];

    vk::DescriptorBufferInfo bufferInfo(uniformBuffer.buffer, 0, alignedUBOSize);
    vk::DescriptorBufferInfo perMeshBufferInfo(scene.perMeshBuffer.buffer, 0, sizeof(PerMeshData) * scene.perMeshData.size());
    vk::DescriptorBufferInfo perInstanceBufferInfo(scene.perInstanceBuffer.buffer, 0, sizeof(PerInstanceData) * scene.perInstanceData.size());
    vk::DescriptorBufferInfo lightSourcesBufferInfo(scene.lightSourcesBuffer.buffer, 0, sizeof(LightSource) * scene.lightSources.size());
    vk::DescriptorImageInfo shadowMapDescriptorImageInfo(VK_NULL_HANDLE, momentImageView, vk::ImageLayout::eGeneral);
    vk::DescriptorBufferInfo doubleMomentBufferInfo(doubleMomentsBuffer.buffer, 0, doubleMomentsBuffer.size);

    std::array descriptorWrites = {
        vk::WriteDescriptorSet(descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBufferDynamic, nullptr, &bufferInfo),
        vk::WriteDescriptorSet(descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perMeshBufferInfo),
        vk::WriteDescriptorSet(descriptorSet, 2, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &perInstanceBufferInfo),
        vk::WriteDescriptorSet(descriptorSet, 3, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr,  &lightSourcesBufferInfo),
        vk::WriteDescriptorSet(descriptorSet, 4, 0, 1, vk::DescriptorType::eStorageImage, &shadowMapDescriptorImageInfo),
        vk::WriteDescriptorSet(descriptorSet, 5, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &doubleMomentBufferInfo)
    };
    app.device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void VarianceShadowMap::createResources(Application &app, uint32_t width) {

    // Image
    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo({}, vk::ImageType::e2D, vk::Format::eR32G32Sfloat, vk::Extent3D(width, width, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);
    VmaAllocationCreateInfo allocationCreateInfo = {.usage = VMA_MEMORY_USAGE_GPU_ONLY};
    VkImage imageOld;
    vmaCreateImage(app.vmaAllocator, &imageCreateInfo, &allocationCreateInfo, &imageOld, &momentImageAllocation, nullptr);
    momentImages = vk::Image(imageOld);
    app.setNameOfObject(momentImages, "Image: Variance shadow map moment images");

    // Transition layout
    vk::CommandBuffer commandBuffer = app.beginSingleTimeCommands();
    vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier({}, vk::AccessFlagBits::eShaderWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, momentImages, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader, {}, 0, nullptr, 0, nullptr, 1, &barrier);
    app.endSingleTimeCommands(commandBuffer);

    momentImageView = app.device.createImageView(vk::ImageViewCreateInfo({}, momentImages, vk::ImageViewType::e2D, vk::Format::eR32G32Sfloat, vk::ComponentMapping(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));
    app.setNameOfObject(momentImageView, "ImageView: Variance shadow map moment image view");

    // SAT buffer
    doubleMomentsBuffer = Buffer(app, width * width * sizeof(double) * 2, vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
    app.setNameOfObject(doubleMomentsBuffer.buffer, "Buffer: Variance shadow map double moments buffer");
}
