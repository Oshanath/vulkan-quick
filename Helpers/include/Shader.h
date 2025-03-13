#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

enum class ShaderType
{
	VERTEX,
	FRAGMENT,
	COMPUTE
};

class Shader
{
public:
	Shader(std::string filePath, const vk::Device& device);

	vk::ShaderModule shaderModule;
};