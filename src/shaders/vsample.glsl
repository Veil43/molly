#version 330 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 tex_coord;
out vec3 normal;
out vec3 frag_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    tex_coord = aTexCoord;
    frag_position = vec3(view * model * vec4(aPos, 1.0));
    // Normal needs to be in view space so we don't need to pass the camera position
    // We should receive a normaMatrix as Uniform;
    normal = normalize(mat3(transpose(inverse(view))) * mat3(transpose(inverse(model))) * aNormal);
}
