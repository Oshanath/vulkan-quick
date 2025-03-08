//
// Created by User on 2/6/2025.
//

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>

class RenderPass {
public:
    vk::Device& device;
    vk::RenderPass renderPass;
    vk::AttachmentDescription colorAttachment;
    vk::AttachmentReference colorAttachmentRef;
    vk::SubpassDescription subpass;
    vk::RenderPassCreateInfo renderPassCreateInfo;

    RenderPass(vk::Device& device, vk::Format format);
    void createRenderPass();

    void beginRenderPass(vk::Framebuffer &swapchainFramebuffer, vk::Extent2D extent, vk::CommandBuffer &commandBuffer);
};



#endif //RENDERPASS_H
