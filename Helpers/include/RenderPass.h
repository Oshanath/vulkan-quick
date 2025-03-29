//
// Created by User on 2/6/2025.
//

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>

class RenderPass {
public:
    vk::RenderPass renderPass;
    vk::AttachmentDescription colorAttachment;
    vk::AttachmentReference colorAttachmentRef;
    vk::AttachmentDescription depthAttachment;
    vk::AttachmentReference depthAttachmentRef;
    vk::SubpassDescription subpass;
    vk::RenderPassCreateInfo renderPassCreateInfo;
    std::array<vk::SubpassDependency, 2> dependencies;
    std::vector<vk::AttachmentDescription> attachments;

    RenderPass();
    RenderPass(vk::Device& device, vk::Format format, vk::Format depthFormat);
    void createRenderPass(vk::Device& device);

    void beginRenderPass(vk::Framebuffer &swapchainFramebuffer, vk::Extent2D extent, vk::CommandBuffer &commandBuffer);
};



#endif //RENDERPASS_H
