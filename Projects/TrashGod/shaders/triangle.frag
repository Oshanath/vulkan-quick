#version 460

#extension GL_EXT_debug_printf : require

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define PI 3.14159265359

struct lightSource {
    vec3 color;
    float intensity;
    vec3 position;
    uint type;
    vec3 direction;
    float padding;
};

struct material {
    vec3 albedo;
    float metallic;
    vec3 emissive;
    float roughness;
};

layout( push_constant ) uniform constants
{
    float ambientFactor;
    float lightIntensity;
} PushConstants;

layout(binding=0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 lightViewProj;
    vec4 cameraPos;
} ubo;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec4 fragPos;
layout(location = 2) flat in uint materialIndex;

layout(location = 0) out vec4 outColor;

layout(binding=3) readonly buffer MaterialSSBO {
    material data[];
} materialData;

layout(binding = 4) readonly buffer LightSources {
    lightSource lightSources[];
};

layout(binding = 5, r32f) readonly uniform image2DArray shadowMap;

float calculateAttenuation(vec3 fragPos, vec3 lightPosition, uint lightSourceIndex) {
    if (lightSources[lightSourceIndex].type == DIRECTIONAL_LIGHT) {
        return 1.0;
    } else {
        float distance = length(fragPos - lightPosition);
        return 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    }
}

vec3 calculateWi(uint lightSourceIndex) {
    if (lightSources[lightSourceIndex].type == DIRECTIONAL_LIGHT) {
        return normalize(-lightSources[lightSourceIndex].direction);
    } else {
        return normalize(lightSources[lightSourceIndex].position - vec3(fragPos));
    }
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float ShadowCalculation(uint lightSourceIndex) {
    vec4 fragPosLightSpace = ubo.lightViewProj * fragPos;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    float closestDepth = imageLoad(shadowMap, ivec3(projCoords.xy * 5000, 0)).r;
    float currentDepth = length(vec3(fragPos) - lightSources[lightSourceIndex].position) / 10000.0;
    return currentDepth > closestDepth  ? 1.0 : 0.0;
}

void main() {

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(ubo.cameraPos.xyz - vec3(fragPos));

    vec3  lightColor  = lightSources[0].color;
    float lightIntensity = PushConstants.lightIntensity;
    vec3  wi          = calculateWi(0);
    float cosTheta    = max(dot(N, wi), 0.0);

    vec3 albedo = materialData.data[materialIndex].albedo + materialData.data[materialIndex].emissive;
    float metallic = materialData.data[materialIndex].metallic;
    float roughness = materialData.data[materialIndex].roughness;
    vec3 emissive = materialData.data[materialIndex].emissive;

    vec3 finalColor = vec3(0.0, 0.0f, 0.0);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 1; i++) {
        vec3 L = normalize(lightSources[i].position - vec3(fragPos));
        vec3 H = normalize(V + L);

        float distance = length(lightSources[i].position - vec3(fragPos));
        float attenuation = calculateAttenuation(vec3(fragPos), lightSources[i].position, 0);
        vec3  radiance    = lightColor * lightIntensity * attenuation * cosTheta;

        vec3 F0 = vec3(0.04);
        F0      = mix(F0, albedo, metallic);
        vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular     = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;

        vec3 ambient = vec3(PushConstants.ambientFactor) * albedo;
        float shadow = ShadowCalculation(i);
        vec3 color   = ambient + (1.0 - shadow) * Lo;
        finalColor += color;
    }

    finalColor = finalColor / (finalColor + vec3(1.0));
    outColor = vec4(finalColor, 1.0);

}