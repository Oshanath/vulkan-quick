//
// Created by User on 2/6/2025.
//

#include "RenderPass.h"

RenderPass::RenderPass() {
}

RenderPass::RenderPass(vk::Device& device, vk::Format format, vk::Format depthFormat)
{
    colorAttachment = vk::AttachmentDescription({}, format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    depthAttachment = vk::AttachmentDescription({}, depthFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    depthAttachmentRef = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef, nullptr, &depthAttachmentRef, 0, nullptr);

    std::array attachments = {colorAttachment, depthAttachment};
    renderPassCreateInfo = vk::RenderPassCreateInfo({}, attachments.size(), attachments.data(), 1, &subpass);
}

void RenderPass::createRenderPass(vk::Device& device) {
    dependency = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        {},
        vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    this->renderPass = device.createRenderPass(renderPassCreateInfo);
}

void RenderPass::beginRenderPass(vk::Framebuffer& swapchainFramebuffer, vk::Extent2D extent, vk::CommandBuffer& commandBuffer) {
    std::array<vk::ClearValue, 2> clearValues = {
        vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})),
        vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0))
    };
    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, swapchainFramebuffer, vk::Rect2D({0, 0}, extent), clearValues.size(), clearValues.data());
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}