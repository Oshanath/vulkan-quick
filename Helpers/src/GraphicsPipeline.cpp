//
// Created by User on 2/4/2025.
//

#include "GraphicsPipeline.h"

#include "RenderPass.h"

GraphicsPipeline::GraphicsPipeline(const vk::Device& device, RenderPass renderPass, uint32_t width, uint32_t height)
{
    dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo({}, dynamicStates);
    vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo({}, 0, nullptr, 0, nullptr);
    inputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);
    viewport = vk::Viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
    scissor = vk::Rect2D({0, 0}, {width, height});
    viewportStateCreateInfo = vk::PipelineViewportStateCreateInfo({}, 1, &viewport, 1, &scissor);
    rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eLine, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
    multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE);
    colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState(VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA));
    colorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo({}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachmentState, {0.0f, 0.0f, 0.0f, 0.0f});
    pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo({}, 0, nullptr, 0, nullptr);
    depthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo({}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f);
    pipelineCreateInfo = vk::GraphicsPipelineCreateInfo({}, shaderStages.size(), shaderStages.data(), &vertexInputStateCreateInfo, &inputAssemblyStateCreateInfo, nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo, &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &colorBlendStateCreateInfo, &dynamicStateCreateInfo, pipelineLayout, renderPass.renderPass, 0, nullptr, -1);
}
void GraphicsPipeline::createLayoutAndPipeline(vk::Device& device) {
    this->pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

    this->pipelineCreateInfo.layout = pipelineLayout;
    this->pipelineCreateInfo.stageCount = shaderStages.size();
    this->pipelineCreateInfo.pStages = shaderStages.data();
    this->graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
}

void GraphicsPipeline::setVertexShader(Shader &shader) {
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, shader.shaderModule, "main");
    shaderStages.push_back(shaderStageCreateInfo);
}

void GraphicsPipeline::setFragmentShader(Shader &shader) {
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, shader.shaderModule, "main");
    shaderStages.push_back(shaderStageCreateInfo);
}
