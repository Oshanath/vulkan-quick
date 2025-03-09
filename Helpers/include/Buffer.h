//
// Created by User on 3/9/2025.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>

class Buffer {
public:
    vk::Buffer buffer;
    VmaAllocation allocation;

    Buffer(VmaAllocator& allocator, size_t size, vk::BufferUsageFlagBits bufferUsage, VmaMemoryUsage memoryUsage);
    void copyData(VmaAllocator& allocator, void* data, size_t size);
};



#endif //BUFFER_H
