#version 460

#extension GL_EXT_debug_printf : require

struct lightSource {
    vec3 color;
    float intensity;
    vec3 position;
    uint type;
    vec3 direction;
    float padding;
};

layout(location = 0) in vec3 fragPos;

layout(location = 0) out vec2 moments;

layout(binding = 3) readonly buffer LightSources {
    lightSource lightSources[];
};

void main() {
    ivec3 coords = ivec3(gl_FragCoord.xy, 0);
    float depth = length(fragPos - lightSources[0].position);
    depth = depth / 10000.0;
    moments = vec2(depth, depth * depth);
}