#ifndef MOLLY_ASSET_REGISTRY_H
#define MOLLY_ASSET_REGISTRY_H

#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <optional>
#include <filesystem>
#include <utility>

#include "types.h"
#include "molly_math.h"
#include "textfile.h"
#include "handle.h"

namespace fs = std::filesystem;

struct Image {
    std::unique_ptr<u8[]> image_data;
    u32 gl_texture_id;
    u32 width;
    u32 height;
    u32 bit_count_per_channel;
    u32 channel_count;
};

struct Texture {
    ARegHandle h_image;
    u32 min_filter;
    u32 mag_filter;
    u32 wrap_s;
    u32 wrap_t;
};

struct Material {
    glm::vec4 diffuse_factor;
    ARegHandle h_diffuse_map_texture;
    ARegHandle h_metallic_roughness_map_texture;
    ARegHandle h_normal_map_texture;
    f32 metallic_factor;
    f32 roughness_factor;
};

struct StaticMesh {
    std::string name;
    glm::mat4 world_transform;
    std::unique_ptr<Vertex[]> vertex_data;
    std::unique_ptr<u32[]> index_data;
    ARegHandle h_material;
    u32 gl_vao_id;
    u32 vertex_count;
    u32 index_count;
};

struct Model {
    std::vector<ARegHandle> h_meshes;
    std::string name;
    glm::mat4 world_transform;
};

struct Asset {
    std::string name;
    std::string path;
    std::string path_raw;
    // Type
    ARegHandle asset_data_id;
};


// NOTE:  At some point we will want to implement these arenas an pools I've been hearing about
// At that point we may want to start doing batch allocations and deletion because things will be in contiguous memory chuncks.
// But for now std::vector and std::array will do the trick
struct AssetRegistry {
public:
    std::string cache_dir;
    std::string asset_dir;
    // ids go from 0 -> 7  for the AssetTypes starting at 0 for shader and 7 for asset.
    //                              gives an array of 8     gives a handle
    // validation looks like m_live_handles[handle.index()][handle.type()].generation() == handle.generation()
    //                  index       handles
    std::vector<std::array<ARegHandle, MOLLY_ASSET_TYPE_COUNT>> m_live_handles;
    std::vector<ARegHandle> m_handle_deletion_stage;

    //          type        free_indices
    std::vector<std::queue<u32>> m_free_handle_indices;

    std::unordered_map<std::string, std::pair<std::string, u64>> m_asset_cache;
    std::vector<TextFile> m_text_files;
    std::vector<Image> m_images;
    std::vector<Texture> m_textures;
    std::vector<Material> m_materials;
    std::vector<StaticMesh> m_static_meshes;
    std::vector<Model> m_models;
    // TODO: add a name -> handle map and a getAssetByName function;

    auto init(const std::string& cache_path, const std::string& asset_path, bool load_cache = true) -> bool;

    /*
        @param str: name or path to be loaded
        @param asset_type: MOLLY_ASSET_...
    */
    auto loadAsset(const std::string& str, u32 asset_type, const std::string& name) -> ARegHandle;   // Store it into the respective array, generate a handle and store it AND 
                                                                // if possible store paths of binary and raw in cache
    auto  storeAsset(TextFile&& file) -> ARegHandle;

    /*
    binary file at: asset_dir/asset.name.asset
    */
    auto writeAssetToBinary(ARegHandle h) -> std::string;
    auto readAssetFromBinary(const std::string& path, u32 type) -> ARegHandle;

    // TODO: Add Generation checking
    auto getTextFile(ARegHandle h) -> std::shared_ptr<TextFile>;
    auto loadCache(const std::string& src) -> std::unordered_map<std::string, std::pair<std::string, u64>>;
    auto storeCache(const std::string& dest) -> void;
    auto deleteAsset(ARegHandle) -> auto;   // TODO: (Later) maybe we could add some sort of deletion queue so that we can delete a bunch all at once. Also invalidate the handle
    auto clearDeletedAssets() -> auto;

    auto inCache(const std::string& path) -> bool { return this->m_asset_cache.find(path) != this->m_asset_cache.end(); }    
};

#endif // MOLLY_ASSET_REGISTRY_H
