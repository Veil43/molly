#version 330 core
out vec4 FragColor;
struct PBRMaterial {
    sampler2D diffuse_map;
    sampler2D metallic_roughness_map;
    sampler2D normal_map;
    sampler2D emissive_map;
    sampler2D occlusion_map;
    vec4 diffuse_color_factor;
    float metallic_factor;
    float roughness_factor;
};

in vec3 normal;
in vec2 tex_coord;

uniform PBRMaterial pbr_material;
void main() {
    FragColor = texture(pbr_material.diffuse_map, tex_coord);
}