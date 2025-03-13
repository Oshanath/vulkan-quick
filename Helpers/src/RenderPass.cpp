//
// Created by User on 2/6/2025.
//

#include "RenderPass.h"

RenderPass::RenderPass(vk::Device& device, vk::Format format)
{
    colorAttachment = vk::AttachmentDescription({}, format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef, nullptr, nullptr, 0, nullptr);
    renderPassCreateInfo = vk::RenderPassCreateInfo({}, 1, &colorAttachment, 1, &subpass);
}

void RenderPass::createRenderPass(vk::Device& device) {
    vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    this->renderPass = device.createRenderPass(renderPassCreateInfo);
}

void RenderPass::beginRenderPass(vk::Framebuffer& swapchainFramebuffer, vk::Extent2D extent, vk::CommandBuffer& commandBuffer) {
    vk::ClearValue clearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, swapchainFramebuffer, vk::Rect2D({0, 0}, extent), 1, &clearValue);
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}