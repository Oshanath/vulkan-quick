//
// Created by User on 3/9/2025.
//

#include "Buffer.h"

#include <vma/vk_mem_alloc.h>

Buffer::Buffer(MainLoop& app, size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage) {
    VkBufferCreateInfo vertexBufferCreateInfo = vk::BufferCreateInfo({}, size, bufferUsage, vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = memoryUsage;
    VmaAllocation allocation;
    VkBuffer vertexBufferOld;
    vmaCreateBuffer(app.vmaAllocator, &vertexBufferCreateInfo, &allocationCreateInfo, &vertexBufferOld, &allocation, nullptr);
    this->buffer = vk::Buffer(vertexBufferOld);
    this->allocation = allocation;
}

void Buffer::copyData(VmaAllocator&allocator, void *data, size_t size) {
    void* mappedData = nullptr;
    vmaMapMemory(allocator, this->allocation, &mappedData);
    memcpy(mappedData, data, size);
    vmaUnmapMemory(allocator, this->allocation);
    vmaFlushAllocation(allocator, this->allocation, 0, size);
}
