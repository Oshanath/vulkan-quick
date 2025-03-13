//
// Created by User on 3/13/2025.
//

#ifndef IMAGE_H
#define IMAGE_H

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include "stb_image.h"
#include "Application.h"
#include "Buffer.h"

class Image {
public:
    vk::Image image;
    VmaAllocation imageAllocation;
    vk::ImageView imageView;

    Image();
};

static Image createImageFromFile(Application& app, std::string path) {

  	Image image;

    int width, height, channels;
    stbi_uc* pixels = stbi_load("Resources/texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = width * height * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    Buffer stagingBuffer = Buffer(app, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.copyData(app.vmaAllocator, pixels, imageSize);
    stbi_image_free(pixels);

    vk::Format imageFormat = vk::Format::eR8G8B8A8Srgb;
    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo({}, vk::ImageType::e2D, imageFormat, vk::Extent3D(width, height, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);
    VmaAllocationCreateInfo allocationCreateInfo = {.usage = VMA_MEMORY_USAGE_GPU_ONLY};
    VkImage imageOld;
    vmaCreateImage(app.vmaAllocator,&imageCreateInfo, &allocationCreateInfo, &imageOld, &image.imageAllocation, nullptr);
    image.image = vk::Image(imageOld);

    vk::CommandBuffer commandBuffer = app.beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier({}, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, &barrier);

    vk::BufferImageCopy region = vk::BufferImageCopy(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), vk::Offset3D(0, 0, 0), vk::Extent3D(width, height, 1));
    commandBuffer.copyBufferToImage(stagingBuffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    vk::ImageMemoryBarrier barrier2 = vk::ImageMemoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, 0, nullptr, 0, nullptr, 1, &barrier2);

    app.endSingleTimeCommands(commandBuffer);

    image.imageView = app.device.createImageView(vk::ImageViewCreateInfo({}, image.image, vk::ImageViewType::e2D, imageFormat, vk::ComponentMapping(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)));

    return image;
}

static Image createDepthImage(Application& app) {
    Image image;
    VkImageCreateInfo imageCreateInfo = vk::ImageCreateInfo({}, vk::ImageType::e2D, app.depthFormat, vk::Extent3D(app.width, app.height, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined);
    VmaAllocationCreateInfo allocationCreateInfo = {.usage = VMA_MEMORY_USAGE_GPU_ONLY};
    VkImage imageOld;
    vmaCreateImage(app.vmaAllocator, &imageCreateInfo, &allocationCreateInfo, &imageOld, &image.imageAllocation, nullptr);
    image.image = vk::Image(imageOld);
    image.imageView = app.device.createImageView(vk::ImageViewCreateInfo({}, image.image, vk::ImageViewType::e2D, vk::Format::eD32Sfloat, vk::ComponentMapping(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)));
    return image;
}

#endif //IMAGE_H
