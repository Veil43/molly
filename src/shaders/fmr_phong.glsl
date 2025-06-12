#version 330 core
out vec4 FragColor;

in vec3 normal;

in vec3 surface_point;
in vec2 tex_coord;

uniform sampler2D diffuse_map;
uniform sampler2D metallic_roughness_map;
uniform sampler2D normal_map;
uniform vec4 diffuse_factor;
uniform float metallic_factor;
uniform float roughness_factor;

uniform vec3 point_light1_position;
uniform vec3 camera_position;

#define MAX_SHININESS (32.0)

void main() {
    vec3 light_dir = normalize(point_light1_position - surface_point);
    vec3 cam_dir = normalize(camera_position - surface_point);
    vec3 reflected_dir = (reflect(-point_light1_position, normal));

    vec2 metallic_roughness = texture(metallic_roughness_map, tex_coord).bg;

    float metalness = mix(0.01, 1.0, metallic_factor * metallic_roughness.x);
    float shininess = (1.0 - roughness_factor * metallic_roughness.y);

    float amb = 0.1;
    float diff = max(dot(light_dir, normal), 0.0);
    float spec = metalness * pow(max(dot(reflected_dir, cam_dir), 0.0), shininess);

    vec3 color = vec3(texture(diffuse_map, tex_coord)) * diff * vec3(diffuse_factor);
    color += vec3(texture(diffuse_map, tex_coord)) * amb;
    color += vec3(1.0) * spec;

    FragColor = vec4(color, 1.0);
}