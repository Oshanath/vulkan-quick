//
// Created by User on 3/16/2025.
//

#ifndef SCENE_H
#define SCENE_H

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
    glm::vec3 albedo;
    float metallic;
    glm::vec3 emissive;
    float roughness;
};

class Mesh {
public:
    uint32_t vertexOffset;
    uint32_t indexOffset;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t materialIndex;
    std::vector<PerInstanceData> perInstanceData;

    Mesh(){}
};

class Model {
public:
    float scaling = 1.0f;
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::vec3 translation = glm::vec3(0.0f);

    std::vector<Mesh> meshes;
};

static glm::mat4 convertAssimpMat4ToGlm(const aiMatrix4x4 &m) {
    return glm::transpose(glm::make_mat4(&m.a1));
}

enum LightSourceType {
    DIRECTIONAL_LIGHT = 0,
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2
};

class LightSource {
public:
    glm::vec3 color;
    float intensity;
    glm::vec3 position;
    uint32_t type;
    glm::vec3 direction;
    float padding;

    LightSource(glm::vec3 color, float intensity, glm::vec3 position, glm::vec3 direction, uint32_t type) :
        color(color), intensity(intensity), position(position), direction(direction), type(type) {}
};

class Scene {
public:
    std::vector<Model> models;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    std::vector<PerInstanceData> perInstanceData;
    std::vector<PerMeshData> perMeshData;
    uint32_t meshCount = 0;
    std::vector<LightSource> lightSources;

    Buffer indirectCommandsBuffer;
    Buffer perInstanceBuffer;
    Buffer perMeshBuffer;
    Buffer materialBuffer;
    Buffer lightSourcesBuffer;
    Buffer vertexBuffer;
    Buffer indexBuffer;

    Model* addModel(Application& app, std::string path, float scaling = 1.0f, glm::mat4 rotation = glm::mat4(1.0f), glm::vec3 translation = glm::vec3(0.0f));
    void generateBuffers(Application& app);
};



#endif //SCENE_H
