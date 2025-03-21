#version 460

#extension GL_EXT_debug_printf : require

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct lightSource {
    vec3 color;
    float intensity;
    vec3 position;
    uint type;
    vec3 direction;
    float padding;
};

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 5) readonly buffer LightSources {
    lightSource lightSources[];
};

void main() {
    debugPrintfEXT("Light color: %f %f %f, intensity: %f, position: %f %f %f, direction: %f %f %f, type: %u\n", lightSources[0].color.x, lightSources[0].color.y, lightSources[0].color.z, lightSources[0].intensity, lightSources[0].position.x, lightSources[0].position.y, lightSources[0].position.z, lightSources[0].direction.x, lightSources[0].direction.y, lightSources[0].direction.z, lightSources[0].type);
    outColor = vec4(fragColor * lightSources[0].color, 1.0);
}