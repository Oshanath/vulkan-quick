#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <string>

Shader::Shader(ShaderType type, std::string filePath)
{
    // Read file into a string
    auto absolutePath = std::filesystem::absolute(filePath);
    std::ifstream file(absolutePath);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    std::string source((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();


}
