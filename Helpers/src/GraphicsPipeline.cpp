//
// Created by User on 2/4/2025.
//

#include "GraphicsPipeline.h"

#include "RenderPass.h"

GraphicsPipeline::GraphicsPipeline(vk::Device& device, RenderPass renderPass, uint32_t width, uint32_t height): device(device),
    dynamicStates({vk::DynamicState::eViewport, vk::DynamicState::eScissor}),
    dynamicStateCreateInfo({}, dynamicStates),
    vertexInputStateCreateInfo({}, 0, nullptr, 0, nullptr),
    inputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE),
    viewport(0.0f, 0.0f, width, height, 0.0f, 1.0f),
    scissor({0, 0}, {width, height}),
    viewportStateCreateInfo({}, 1, &viewport, 1, &scissor),
    rasterizationStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f),
    multisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE),
    colorBlendAttachmentState(VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)),
    colorBlendStateCreateInfo({}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachmentState, {0.0f, 0.0f, 0.0f, 0.0f}),
    pipelineLayoutCreateInfo({}, 0, nullptr, 0, nullptr),
    pipelineCreateInfo({}, shaderStages.size(), shaderStages.data(), &vertexInputStateCreateInfo, &inputAssemblyStateCreateInfo, nullptr, &viewportStateCreateInfo, &rasterizationStateCreateInfo, &multisampleStateCreateInfo, nullptr, &colorBlendStateCreateInfo, &dynamicStateCreateInfo, pipelineLayout, renderPass.renderPass, 0, nullptr, -1)
{

}

void GraphicsPipeline::createLayoutAndPipeline() {
    this->pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

    this->pipelineCreateInfo.layout = pipelineLayout;
    this->pipelineCreateInfo.stageCount = shaderStages.size();
    this->pipelineCreateInfo.pStages = shaderStages.data();
    this->graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
}

void GraphicsPipeline::setVertexShader(Shader &shader) {
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, shader.getShaderModule(), "main");
    shaderStages.push_back(shaderStageCreateInfo);
}

void GraphicsPipeline::setFragmentShader(Shader &shader) {
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, shader.getShaderModule(), "main");
    shaderStages.push_back(shaderStageCreateInfo);
}
