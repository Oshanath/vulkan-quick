#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_debug_printf : require

struct perMeshData {
    uint startInstance;
    uint materialIndex;
};

struct perInstanceData {
    mat4 model;
};

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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData.data[meshIndex].model;
    gl_Position = ubo.lightViewProj * model * vec4(inPosition, 1.0);

    fragPos = vec3(model * vec4(inPosition, 1.0));
}