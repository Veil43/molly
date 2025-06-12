#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "molly_math.h"

namespace gltf 
{

enum class eBasicType {
    kByte,
    kUnsignedByte,
    kShort,
    kUnsignedShort,
    kUnsignedInt,
    kFloat,
};

struct TextureInfo {
    std::string name;
    std::string path;
};

struct MaterialInfo {
    std::string name;
    TextureInfo diffuse_map;
    TextureInfo metallic_roughness_map;
    TextureInfo normal_map;
    glm::vec4 diffuse_factor = glm::vec4(1.0f);
    float metallic_factor = 1.0f;
    float roughness_factor = 1.0f;
    float normal_scale = 1.0f;
};

struct MeshData {
    std::string name;
    Transform transform;
    std::vector<unsigned char> pos_data;
    std::vector<unsigned char> nor_data;
    std::vector<unsigned char> tex_data_0;
    std::vector<unsigned char> indices_data;
    int material_index;
    eBasicType indices_type;
    eBasicType texture_type_0;
};

struct ModelData {
    std::string name;
    std::vector<MeshData> meshes;
};

struct SceneData {
    std::vector<ModelData> models;
    std::vector<MaterialInfo> materials;
};

SceneData load_gltf_file(const std::filesystem::path& path);

} // namespace gltf