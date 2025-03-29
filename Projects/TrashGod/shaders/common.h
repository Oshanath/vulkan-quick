struct perMeshData {
    uint startInstance;
    uint materialIndex;
};

struct perInstanceData {
    mat4 model;
};

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