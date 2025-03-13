//
// Created by User on 2/4/2025.
//

#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.hpp>

#include "RenderPass.h"
#include "Shader.h"


class GraphicsPipeline {
public:
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    std::vector <vk::PipelineShaderStageCreateInfo> shaderStages;
    std::vector<vk::DynamicState> dynamicStates;
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    vk::PipelineLayout pipelineLayout;
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    vk::Viewport viewport;
    vk::Rect2D scissor;
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
    vk::Pipeline graphicsPipeline;

    explicit GraphicsPipeline(const vk::Device& device, RenderPass renderPass, uint32_t width, uint32_t height);

    void createLayoutAndPipeline(vk::Device& device);
    void setVertexShader(Shader& shader);
    void setFragmentShader(Shader &shader);
};



#endif //PIPELINE_H
