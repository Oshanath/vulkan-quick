//
// Created by User on 3/9/2025.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include "MainLoop.h"

class Buffer {
public:
    vk::Buffer buffer;
    VmaAllocation allocation;

    Buffer(MainLoop& app, size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
    void copyData(VmaAllocator& allocator, void* data, size_t size, size_t offset = 0);
};

static Buffer createBufferWithData(MainLoop& app, size_t size, vk::BufferUsageFlagBits bufferUsage, VmaMemoryUsage memoryUsage, void* data) {
    Buffer stagingBuffer(app, size, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
    stagingBuffer.copyData(app.vmaAllocator, data, size);

    Buffer buffer(app, size, bufferUsage | vk::BufferUsageFlagBits::eTransferDst, memoryUsage);
    vk::CommandBuffer commandBuffer = app.beginSingleTimeCommands();
    vk::BufferCopy copyRegion(0, 0, size);
    commandBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, copyRegion);
    commandBuffer.end();
    app.graphicsQueue.submit({vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr)}, nullptr);
    app.graphicsQueue.waitIdle();
    return buffer;
}



#endif //BUFFER_H
