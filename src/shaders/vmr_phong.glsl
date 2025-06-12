#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aTangent;
layout (location = 3) in vec2 aTexCoord;

out vec3 normal;

out vec3 surface_point;
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    surface_point = vec3(model * vec4(aPos, 1.0));

    mat3 normal_matrix = mat3(transpose(inverse(model)));

    normal = normalize(normal_matrix * aNormal);

    tex_coord = aTexCoord;
}
