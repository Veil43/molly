#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec2 tex_coord;

uniform PBRMaterial pbr_material;
uniform sampler2D map_diffuse;
uniform sampler2D map_normal;
uniform sampler2D map_specular;

void main() {
    
    FragColor = 0.5 * (vec4(normal, 1.0) + vec4(1.0));
}