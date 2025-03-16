#version 460

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_debug_printf : require

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
    perMeshData data[];
} meshData;

layout(binding=3) readonly buffer PerInstanceDataSSBO {
    perInstanceData data[];
} instanceData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

void main() {
    uint meshIndex = gl_DrawID;
    uint instanceIndex = gl_InstanceIndex;
    mat4 model = instanceData.data[meshIndex].model;
    debugPrintfEXT("model: %f %f %f %f\n", model[2][0], model[2][1], model[2][2], model[2][3]);
    gl_Position = ubo.proj * ubo.view * ubo.model * model * vec4(inPosition, 1.0);
    fragColor = vec3(1.0, 1.0, 1.0);
}