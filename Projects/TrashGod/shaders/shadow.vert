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
    vec4 cameraPos;
} ubo;

layout(binding=1) readonly buffer PerMeshDataSSBO {
    perMeshData data[];
} meshData;

layout(binding=2) readonly buffer PerInstanceDataSSBO {
    perInstanceData data[];
} instanceData;

layout( push_constant ) uniform constants
{
    mat4 viewProj;
} PushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData.data[meshIndex].model;
    gl_Position = PushConstants.viewProj * model * vec4(inPosition, 1.0);
}