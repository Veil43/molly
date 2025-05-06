#version 330 core
struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

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


struct PointLight {
    vec3 position;
    vec3 attenuation;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 frag_color;

in vec3 normal;
in vec3 frag_position;
in vec2 tex_coord;

uniform PBRMaterial pbr_material;
uniform PointLight point_light1;
uniform vec3 camera_position;
void main() {
    vec3 ambient_light_color = point_light1.ambient;

    // Diffuse color calculation. We need the vertex normal, fragment_position;
    vec3 light_direction = normalize(point_light1.position - frag_position);
    vec3 diffuse_light_color = max(dot(light_direction, normal), 0.0) * point_light1.diffuse;

    float shininess = vec3(texture(pbr_material.metallic_roughness_map, tex_coord)).r;
    // Specular color calculations assumes camera is at 0,0,0
    vec3 reflected = reflect(-light_direction, normal);
    vec3 view_direction = normalize(camera_position-frag_position);
    vec3 specular_light_color = pow(max(dot(reflected, view_direction), 0.0),
                                    shininess)
                                * point_light1.specular;

    vec3 ambient = ambient_light_color * vec3(texture(pbr_material.diffuse_map, tex_coord));
    vec3 diffuse = diffuse_light_color * vec3(texture(pbr_material.diffuse_map, tex_coord));
    vec3 specular = specular_light_color * vec3(texture(pbr_material.metallic_roughness_map, tex_coord)).ggg;

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
