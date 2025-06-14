#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_debug_printf : require

#include "common.h"

layout(binding=0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 lightViewProj;
    vec4 cameraPos;
} ubo;

layout(binding=1) readonly buffer PerMeshDataSSBO {
    perMeshData data[];
} meshData;

layout(binding=2) readonly buffer PerInstanceDataSSBO {
    perInstanceData data[];
} instanceData;

layout(binding=3) readonly buffer MaterialSSBO {
    material data[];
} materialData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec4 fragPos;
layout(location = 2) flat out uint materialIndex;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData.data[meshIndex].model;
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);

    vec3 albedo = materialData.data[meshData.data[meshIndex].materialIndex].albedo;
    vec3 emissive = materialData.data[meshData.data[meshIndex].materialIndex].emissive;

    fragNormal = transpose(inverse(mat3(model))) * inNormal;
    fragPos = model * vec4(inPosition, 1.0);
    materialIndex = meshData.data[meshIndex].materialIndex;
}