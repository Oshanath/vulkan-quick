//
// Created by User on 3/13/2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "Application.h"

class Texture {
public:
    vk::Image image;
    VmaAllocation imageAllocation;
    vk::ImageView imageView;
    vk::Sampler sampler;

    Texture(Application& app, std::string path);
};



#endif //TEXTURE_H
