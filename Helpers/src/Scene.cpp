//
// Created by User on 3/16/2025.
//

#include "Scene.h"

#include <queue>
#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "assimp/mesh.h"

#define VULKAN_HPP_STORAGE_DISPATCH_GLOBALS
#include <vulkan/vulkan.hpp>

Model* Scene::addModel(Application& app, std::string path, float scaling, glm::mat4 rotation, glm::vec3 translation) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    if (scene == nullptr) {
        throw std::runtime_error("Failed to load model: " + path);
    }

    Model model;
    model.scaling = scaling;
    model.rotation = rotation;
    model.translation = translation;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        Mesh mesh;
        mesh.vertexOffset = vertices.size();
        mesh.indexOffset = indices.size();

        aiMesh* aiMesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
            Vertex vertex;
            vertex.position = {aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z};
            vertex.normal = {aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z};
            vertices.push_back(Vertex{vertex.position, vertex.normal});
        }

        for (unsigned int j = 0; j < aiMesh->mNumFaces; j++) {
            aiFace face = aiMesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }

        mesh.indexCount = indices.size() - mesh.indexOffset;
        mesh.vertexCount = vertices.size() - mesh.vertexOffset;
        mesh.materialIndex = aiMesh->mMaterialIndex;
        model.meshes.push_back(mesh);
    }

    std::queue<std::pair<const aiNode*, glm::mat4>> nodes;
    nodes.push({scene->mRootNode, glm::translate(glm::mat4(1.0f), model.translation) * model.rotation * glm::scale(glm::mat4(1.0f), glm::vec3(model.scaling)) * convertAssimpMat4ToGlm(scene->mRootNode->mTransformation)});

    while (!nodes.empty()) {
        std::pair<const aiNode*, glm::mat4> entry = nodes.front();
        nodes.pop();
        const aiNode* node = entry.first;
        glm::mat4 parentTransform = entry.second;

        for (int i = 0; i < node->mNumChildren; i++) {
            aiNode* child = node->mChildren[i];
            glm::mat4 transform = parentTransform * convertAssimpMat4ToGlm(child->mTransformation);
            nodes.push({child, transform});
        }

        for (int i = 0; i < node->mNumMeshes; i++) {
            PerInstanceData data;
            data.model = parentTransform;
            model.meshes[node->mMeshes[i]].perInstanceData.push_back(data);
        }
    }

    for (int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        Material mat;

        aiColor3D diffuse;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        mat.albedo = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

        aiColor3D emissive;
        material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        mat.emissive = glm::vec3(emissive.r, emissive.g, emissive.b);

        aiColor3D metallic;
        material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
        mat.metallic = metallic.r;

        aiColor3D roughness;
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
        mat.roughness = roughness.r;

        materials.push_back(mat);
    }

    meshCount += model.meshes.size();
    models.push_back(model);
    return &models.back();
}

void Scene::generateBuffers(Application &app) {
    std::vector<vk::DrawIndexedIndirectCommand> indirectCommands;

    for (Model &model : models) {
        for (Mesh &mesh : model.meshes) {
            perInstanceData.insert(perInstanceData.end(), mesh.perInstanceData.begin(), mesh.perInstanceData.end());
            PerMeshData data;
            data.startInstance = perInstanceData.size() - mesh.perInstanceData.size();
            data.materialIndex = mesh.materialIndex;
            perMeshData.push_back(data);

            vk::DrawIndexedIndirectCommand command;
            command.firstIndex = mesh.indexOffset;
            command.indexCount = mesh.indexCount;
            command.firstInstance = 0;
            command.instanceCount = mesh.perInstanceData.size();
            command.vertexOffset = mesh.vertexOffset;
            indirectCommands.push_back(command);
        }
    }
    indirectCommandsBuffer = createBufferWithData(app, sizeof(vk::DrawIndexedIndirectCommand) * indirectCommands.size(), vk::BufferUsageFlagBits::eIndirectBuffer, VMA_MEMORY_USAGE_GPU_ONLY, indirectCommands.data());
    perInstanceBuffer = createBufferWithData(app, sizeof(PerInstanceData) * perInstanceData.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_GPU_ONLY, perInstanceData.data());
    perMeshBuffer = createBufferWithData(app, sizeof(PerMeshData) * perMeshData.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_GPU_ONLY, perMeshData.data());
    materialBuffer = createBufferWithData(app, sizeof(Material) * materials.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_GPU_ONLY, materials.data());
    lightSourcesBuffer = createBufferWithData(app, sizeof(LightSource) * lightSources.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_GPU_ONLY, lightSources.data());
}
