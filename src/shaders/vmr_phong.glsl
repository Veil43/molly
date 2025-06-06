#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 normal;
out vec3 surface_point;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 view_pos = view * model * vec4(aPos, 1.0);
    gl_Position = projection * view_pos;
    surface_point = view_pos.xyz;
    normal = normalize(mat3(transpose(inverse(model))) * mat3(transpose(inverse(view))) * aNormal);
    tex_coord = aTexCoord;
}
