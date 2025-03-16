//
// Created by User on 3/15/2025.
//

#ifndef MODEL_H
#define MODEL_H
#include <Buffer.h>
#include <string>

#include "assimp/mesh.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "vulkan/vulkan.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        return {
            vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)),
            vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal))
        };
    }
};

struct PerInstanceData {
    glm::mat4 model;
};

struct PerMeshData {
    uint32_t startInstance;
    uint32_t materialIndex;
};

struct Material {
    glm::vec4 diffuse;
};

class Mesh {
public:
    uint32_t vertexOffset;
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t materialIndex;
    std::vector<PerInstanceData> perInstanceData;

    Mesh(){}
};

class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Mesh> meshes;
    std::vector<PerInstanceData> perInstanceData;
    std::vector<PerMeshData> perMeshData;
    std::vector<Material> materials;
    Buffer indirectCommandsBuffer;
    Buffer perInstanceBuffer;
    Buffer perMeshBuffer;
    Buffer materialBuffer;

    Model(){}
    Model(Application& app, std::string path);
};

static glm::mat4 convertAssimpMat4ToGlm(const aiMatrix4x4 &m) {
    return glm::transpose(glm::make_mat4(&m.a1));
}

#endif //MODEL_H
