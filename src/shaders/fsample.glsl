#version 330 core
struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 attenuation;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 attenuation;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outer_cutoff;
};

vec3 computeSpotLightColor();

out vec4 frag_color;

in vec2 tex_coord;
in vec3 normal;
in vec3 frag_position;

uniform Material container;
uniform PointLight point_light1;
uniform SpotLight spot_light1;
uniform vec3 camera_position;

void main() {
    vec3 ambient_light_color = point_light1.ambient;

    // Diffuse color calculation. We need the vertex normal, fragment_position;
    vec3 light_direction = normalize(point_light1.position - frag_position);
    vec3 diffuse_light_color = max(dot(light_direction, normal), 0.0) * point_light1.diffuse;

    // Specular color calculations assumes camera is at 0,0,0
    vec3 reflected = reflect(-light_direction, normal);
    vec3 view_direction = normalize(camera_position-frag_position);
    vec3 specular_light_color = pow(max(dot(reflected, view_direction), 0.0),
                                   container.shininess)
                                * point_light1.specular;

    vec3 ambient = ambient_light_color * vec3(texture(container.diffuse_map, tex_coord));
    vec3 diffuse = diffuse_light_color * vec3(texture(container.diffuse_map, tex_coord));
    vec3 specular = specular_light_color * vec3(texture(container.specular_map, tex_coord));

    vec3 spot_light_color = computeSpotLightColor();
    frag_color = vec4(spot_light_color, 1.0);
}

vec3 computeSpotLightColor() {
    vec3 ambient_color = spot_light1.ambient;

    // vec3 light_dir = normalize(light.position - frag_pos); // << point light/ spot light
    // float theta = dot(light_dir, normalize(-light.direction));
    // float epsilon = light.cutoff - light.outer_cutoff;
    // float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0f);

    vec3 light_direction = normalize(spot_light1.position - frag_position);
    float cos_theta = dot(-light_direction, normalize(spot_light1.direction));
    float epsilon = spot_light1.cutoff - spot_light1.outer_cutoff;
    float intensity = clamp((cos_theta - spot_light1.outer_cutoff) / epsilon, 0.0, 1.0);

    vec3 diffuse_color = max(dot(light_direction, normal), 0.0) * spot_light1.diffuse;

    vec3 reflected = reflect(-light_direction, normal);
    vec3 view_direction = normalize(camera_position - frag_position);
    vec3 specular_color = pow(max(dot(reflected, view_direction), 0.0),
                                    container.shininess)
                          * spot_light1.specular;
    vec3 ambient = ambient_color * vec3(texture(container.diffuse_map, tex_coord));
    vec3 diffuse = diffuse_color * vec3(texture(container.diffuse_map, tex_coord));
    vec3 specular = specular_color * vec3(texture(container.specular_map, tex_coord));

    float distance = length(spot_light1.position - frag_position);
    float attenuation = 1.0 / (spot_light1.attenuation.x + spot_light1.attenuation.y * distance + spot_light1.attenuation.z * (distance *distance));
    // float attenuation = 1.0 / (distance * distance); // for realistic falloff

    vec3 result = ambient + (intensity * attenuation * (diffuse + specular));

    return result;
}