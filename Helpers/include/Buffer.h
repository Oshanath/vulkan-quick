//
// Created by User on 3/9/2025.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include "Application.h"

class Buffer {
public:
    vk::Buffer buffer;
    VmaAllocation allocation;

    Buffer(Application& app, size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
    void copyData(VmaAllocator& allocator, void* data, size_t size, size_t offset = 0);
};

static Buffer createBufferWithData(Application& app, size_t size, vk::BufferUsageFlagBits bufferUsage, VmaMemoryUsage memoryUsage, void* data) {
    Buffer stagingBuffer(app, size, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
    stagingBuffer.copyData(app.vmaAllocator, data, size);

    Buffer buffer(app, size, bufferUsage | vk::BufferUsageFlagBits::eTransferDst, memoryUsage);
    vk::CommandBuffer commandBuffer = app.beginSingleTimeCommands();
    commandBuffer.copyBuffer(stagingBuffer.buffer, buffer.buffer, vk::BufferCopy(0, 0, size));
    app.endSingleTimeCommands(commandBuffer);
    return buffer;
}



#endif //BUFFER_H
