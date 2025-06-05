#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "molly_math.h"

namespace gltf 
{

struct TextureInfo {
    std::string name;
    std::string path;
};

struct MaterialInfo {
    std::string name;
    TextureInfo diffuse_map;
    TextureInfo metallic_roughness_map;
    TextureInfo normal_map;
    glm::vec4 diffuse_factor;
    float metallic_factor;
    float roughness_factor;
    float normal_scale;
};

struct MeshData {
    std::string name;
    glm::mat4 transform;
    std::vector<unsigned char> pos_data;
    std::vector<unsigned char> nor_data;
    std::vector<unsigned char> tex_data;
    std::vector<unsigned char> indices_data;
    int material_index;
};

struct ModelData {
    std::string name;
    std::vector<MeshData> meshes;
};

struct Scene {
    std::vector<ModelData> models;
    std::map<int, MaterialInfo> materials;
};

Scene load_gltf_file(const std::filesystem::path& path);

} // namespace gltf