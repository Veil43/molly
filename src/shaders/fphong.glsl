#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    float shininess;
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

uniform Material material;
uniform PointLight point_light1;
uniform vec3 camera_position;
void main() {
    vec3 ambient_light_color = point_light1.ambient;

    // Diffuse color calculation. We need the vertex normal, fragment_position;
    vec3 light_direction = normalize(point_light1.position - frag_position);
    vec3 diffuse_light_color = max(dot(light_direction, normal), 0.0) * point_light1.diffuse;

    // Specular color calculations assumes camera is at 0,0,0
    vec3 reflected = reflect(-light_direction, normal);
    vec3 view_direction = normalize(camera_position-frag_position);
    vec3 specular_light_color = pow(max(dot(reflected, view_direction), 0.0), material.shininess)
                                * point_light1.specular;

    vec3 ambient = ambient_light_color * vec3(texture(material.texture_diffuse1, tex_coord));
    vec3 diffuse = diffuse_light_color * vec3(texture(material.texture_diffuse1, tex_coord));
    vec3 specular = specular_light_color * vec3(texture(material.texture_specular1, tex_coord));

    // frag_color = vec4(ambient + diffuse + specular, 1.0);
    frag_color = vec4(vec3(texture(material.texture_diffuse1, tex_coord)), 1.0);
}
