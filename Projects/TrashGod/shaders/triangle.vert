#version 460

#extension GL_EXT_nonuniform_qualifier : require

struct perMeshData {
    uint startInstance;
};

struct perInstanceData {
    mat4 model;
};

layout(binding=0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding=2) readonly buffer PerMeshDataSSBO {
    perMeshData data;
} meshData[];

layout(binding=3) readonly buffer PerInstanceDataSSBO {
    perInstanceData data;
} instanceData[];

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData[meshData[meshIndex].data.startInstance].data.model;
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);
    fragColor = inNormal;
}