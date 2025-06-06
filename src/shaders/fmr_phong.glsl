#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec2 tex_coord;

uniform sampler2D diffuse_map;
uniform sampler2D metallic_roughness_map;
uniform sampler2D normal_map;
uniform vec4 diffuse_factor;
uniform float metallic_factor;
uniform float roughness_factor;


void main() {

    FragColor = texture(diffuse_map, tex_coord);
}