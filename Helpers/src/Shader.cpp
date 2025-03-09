#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <string>

Shader::Shader(std::string filePath, const vk::Device& device)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    vk::ShaderModuleCreateInfo shader_module_create_info({}, buffer.size(), reinterpret_cast<const uint32_t*>(buffer.data()));
    this->shaderModule = device.createShaderModule(shader_module_create_info);

}
