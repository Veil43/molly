#version 330 core
out vec4 frag_color;

in vec2 tex_coord;

uniform sampler2D container;
void main() {
    frag_color = texture(container, tex_coord);
}