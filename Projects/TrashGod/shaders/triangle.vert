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

struct material {
    vec4 diffuse;
};

layout(binding=0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding=2) readonly buffer PerMeshDataSSBO {
    perMeshData data[];
} meshData;

layout(binding=3) readonly buffer PerInstanceDataSSBO {
    perInstanceData data[];
} instanceData;

layout(binding=4) readonly buffer MaterialSSBO {
    material data[];
} materialData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData.data[meshIndex].model;
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 1.0);
    fragColor = materialData.data[meshData.data[meshIndex].materialIndex].diffuse.xyz;
    debugPrintfEXT("Color: %f %f %f\n", fragColor.r, fragColor.g, fragColor.b);
}