#ifndef MOLLY_ASSET_REGISTRY_H
#define MOLLY_ASSET_REGISTRY_H

#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <string>
#include <memory>

#include "types.h"
#include "molly_math.h"

namespace tmp {

/*
    Asset Registry Handle (64bits)
    type     generation    index
    [8 bits ][   24 bits ][     32 bits     ]
*/
struct ARegHandle {
    u64 value;
    u32 type() {
        return static_cast<u32>(value >> 56) & 0xFF; 
    }
    u32 generation() {
        return static_cast<u32>(value >> 32) & 0xFFFFFFu;
    }
    u32 index() {
        return static_cast<u32>(value & 0xFFFFFFFFu);
    }
    static ARegHandle make(u32 type, u32 generation, u32 index) {
        u64 val = ((static_cast<u64>(type))       << 56) |
                  ((static_cast<u64>(generation)) << 32) |
                  (static_cast<u64>(index));
                    
        return {val};
    }
};

struct TextFile {
    std::string data;
};

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

#define MOLLY_ASSET_TYPE_COUNT          7u

#define MOLLY_ASSET_TEXT_FILE_TYPE      1u
#define MOLLY_ASSET_IMAGE_TYPE          2u
#define MOLLY_ASSET_TEXTURE_TYPE        3u
#define MOLLY_ASSET_MATERIAL_TYPE       4u
#define MOLLY_ASSET_STATIC_MESH_TYPE    5u
#define MOLLY_ASSET_MODEL_TYPE          6u
// #define MOLLY_ASSET_ASSET_TYPE          255u
#define MOLLY_ASSET_UNKOWN_TYPE         0u

// NOTE:  At some point we will want to implement these arenas an pools I've been hearing about
// At that point we may want to start doing batch allocations and deletion because things will be in contiguous memory chuncks.
// But for now std::vector and std::array will do the trick
class AssetRegistry {
public:
    // ids go from 0 -> 7  for the AssetTypes starting at 0 for shader and 7 for asset.
    //                              gives an array of 8     gives a handle
    // validation looks like m_live_handles[handle.index()][handle.type()].generation() == handle.generation()
    //                  index       handles
    std::vector<std::array<ARegHandle, MOLLY_ASSET_TYPE_COUNT>> m_live_handles;
    std::vector<ARegHandle> m_handle_deletion_stage;

    //          type        free_indices
    std::vector<std::queue<u32>> m_free_handle_indices;
    

    std::unordered_map<std::string, std::string> m_asset_cache; // raw path to binary path
    std::vector<TextFile> m_text_files;                         // a name, a path, a shader id, a gl-program id
    std::vector<Image> m_images;                                // a name, a path, an image id, a buffer, a gl texture id
    std::vector<Texture> m_textures;                            // a name, an image id, an id, a min/mag filter, a wrap s/t
    std::vector<Material> m_materials;                          // a name, a material id, a texture id for: diffuse map, metallic roughness map, normal map, 
                                                                // a vector for diffuse factor, a float for metallic factor, a float for roughness factor
    std::vector<StaticMesh> m_static_meshes;                    // a name, a mesh id, a material id, a vao, a transform, a data buffer
    std::vector<Model> m_models;                                // a name, a model id, a transform, a list of mesh ids, 
    // std::vector<Asset> m_assets;                                // a name, a path, an asset id, a model id, a shader id, a raw path

    /*
        @param str: name or path to be loaded
        @param option: `0` for path and `1` for name.
    */
    ARegHandle loadAsset(const std::string& str, u32 asset_type);   // Store it into the respective array, generate a handle and store it AND 
                                                                // if possible store paths of binary and raw in cache
    void deleteAsset(ARegHandle);   // TODO: (Later) maybe we could add some sort of deletion queue so that we can delete a bunch all at once. Also invalidate the handle
    bool cacheAsset(ARegHandle);    // TODO: maybe see what actual params you need for implementation

private:
    void clearDeletedAssets();
    void loadCache();
    ARegHandle loadAssetBin(const std::string& str, u32 type) {return {};}
    ARegHandle storeAsset(TextFile&& file);
};
} // namespace tmp
#endif // MOLLY_ASSET_REGISTRY_H
