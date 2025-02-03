#pragma once

#include <string>

enum class ShaderType
{
	VERTEX,
	FRAGMENT,
	COMPUTE
};

class Shader
{
public:
	Shader(ShaderType type, std::string filePath);
};