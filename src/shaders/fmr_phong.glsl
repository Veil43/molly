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

void main() {
    vec3 light_dir = normalize(point_light1_position - surface_point);

    float amb = 0.0;
    float diff = max(dot(light_dir, normal), 0.0);
    
    vec3 color = vec3(texture(diffuse_map, tex_coord)) * diff;
    color += vec3(texture(diffuse_map, tex_coord)) * amb;
    
    FragColor = vec4(color, 1.0);
}