#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

vec4 color_inverse();
vec4 grey_scale();

uniform sampler2D screen_texture;
void main() {
    FragColor = color_inverse();
    FragColor = grey_scale();
}

vec4 color_inverse() {
    return vec4(vec3(1.0 - texture(screen_texture, tex_coord)), 1.0);
}

vec4 grey_scale() {
    vec4 color = texture(screen_texture, tex_coord);
    float average = (color.r + color.g + color.b) / 3.0;
    return vec4(vec3(average), 1.0);
}