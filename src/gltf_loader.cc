#include "gltf_loader.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
using namespace tinygltf;
using Root = Model;

#include <cstdio>
#include <cassert>

#include "molly_math.h"

namespace fs = std::filesystem;

static std::map<int, gltf::MaterialInfo> 
g_material_cache;

static std::string 
g_base_asset_dir = "";

static gltf::TextureInfo
get_texture_info(const Root& root, int index) {
    gltf::TextureInfo output_texture = {};

    assert(index >= 0);
    const Texture& texture = root.textures[index];
    assert(texture.source >= 0);

    output_texture.name = texture.name;
    output_texture.path = g_base_asset_dir + "/" +  root.images[texture.source].uri;
    /// TODO: add this to the texture data once you want to enable multiple uv sets
    // int tex_coord_set = pbr_mr.baseColorTexture.texCoord;

    return output_texture;
}

static std::vector<unsigned char>
get_buffer_data_from_accessor(const Root& root, int acc_index) {
    std::vector<unsigned char> output_buffer;
    assert(acc_index >= 0);
    const Accessor& accessor = root.accessors[acc_index];
    const BufferView& buffer_view = root.bufferViews[accessor.bufferView];
    const Buffer& buffer = root.buffers[buffer_view.buffer];

    int component_size  = 1;
    int component_count = 1;
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_BYTE:              { component_size = 1; } break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:     { component_size = 1; } break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:             { component_size = 2; } break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:    { component_size = 2; } break;
        case TINYGLTF_COMPONENT_TYPE_INT:               { component_size = 4; } break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:      { component_size = 4; } break;
        case TINYGLTF_COMPONENT_TYPE_FLOAT:             { component_size = 4; } break;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE :           { component_size = 8; } break;
    }

    switch (accessor.type) {
        case TINYGLTF_TYPE_SCALAR:  { component_count = 1;  } break;
        case TINYGLTF_TYPE_VEC2:    { component_count = 2;  } break;
        case TINYGLTF_TYPE_VEC3:    { component_count = 3;  } break;
        case TINYGLTF_TYPE_VEC4:    { component_count = 4;  } break;
        case TINYGLTF_TYPE_MAT2:    { component_count = 4;  } break;
        case TINYGLTF_TYPE_MAT3:    { component_count = 9;  } break;
        case TINYGLTF_TYPE_MAT4:    { component_count = 16; } break;
    }

    const unsigned char* data = &buffer.data[accessor.byteOffset + buffer_view.byteOffset];
    output_buffer.reserve(component_size * component_count *  accessor.count);

    for (int i = 0; i < accessor.count * component_count * component_size; i++) {
        output_buffer.push_back(data[i]);
    }

    return output_buffer;
}

static std::vector<gltf::MeshData> 
load_primitive_meshes(const Root& root, int mesh_id) {
    /*
    *Mesh*
        - primitives[accessorIDs]
        - name
    
    *Mesh Primitive*
        - attributes {
            "POSITION": - accessorID (vec3f)
            "NORMAL": - accessorID (vec3f)
            "TANGENT": -accessorID (vec3f)
            "TEXCOORD_0": -accessorID (vec2) f?i8?u8?i16?u16?
            "TEXCOORD_1": -accessorID (vec2) f?i8?u8?i16?u16?
        }
        - indices  - accessorID (u32)
        - material - materialID
        - mode : assumes GL_TRINAGLES
    
    *Material*
    - name
    - pbrMetallicRoughNess {
        baseColorFactor: vec4
        baseColorTexture: textureInfo
        metallicFactor: int
        roughnessFator: int
        metallicRoughnessTexture: textureInfo
    }
    - normalTexture {
        index: textureID
        texCoord: int
        scale: int
    }

    *TextureInfo*
        - index: textureID
        - texCoord: int
    */
    
    std::vector<gltf::MeshData> output_meshes = {};
    assert(mesh_id >= 0);
    const Mesh& mesh = root.meshes[mesh_id];
    output_meshes.reserve(mesh.primitives.size());

    int primitive_id = 0;
    for (const auto& primitive: mesh.primitives) {
        gltf::MeshData curr_mesh = {};
        curr_mesh.name = mesh.name + std::to_string(primitive_id);
        // ----------------------------------------------------
        // Mesh
        // ----------------------------------------------------
        /// TODO: add full support for tangent and other texture coordinates
        int pos_accessor_index = primitive.attributes.at("POSITION");
        int nor_accessor_index = primitive.attributes.at("NORMAL");
        // int tan_accessor_index = primitive.attributes.at("TANGENT");
        int tex_accessor_index_0 = primitive.attributes.at("TEXCOORD_0");
        // int tex_accessor_index_1 = primitive.attributes.at("TEXCOORD_");
        int indices_accessor_index = primitive.indices;

        curr_mesh.pos_data = get_buffer_data_from_accessor(root, pos_accessor_index);
        curr_mesh.nor_data = get_buffer_data_from_accessor(root, nor_accessor_index);
        curr_mesh.tex_data = get_buffer_data_from_accessor(root, tex_accessor_index_0);
        curr_mesh.indices_data = get_buffer_data_from_accessor(root, indices_accessor_index);

        // ----------------------------------------------------
        // Material
        // ----------------------------------------------------
        gltf::MaterialInfo curr_material;
        assert(primitive.material >= 0);
        const Material& material =  root.materials[primitive.material];
        const PbrMetallicRoughness& pbr_mr = material.pbrMetallicRoughness;
        
        curr_material.name = material.name;
        curr_material.diffuse_map = get_texture_info(root, pbr_mr.baseColorTexture.index);
        curr_material.metallic_roughness_map = get_texture_info(root, pbr_mr.metallicRoughnessTexture.index);
        curr_material.normal_map = get_texture_info(root, material.normalTexture.index);
        curr_material.diffuse_factor = glm::vec4(pbr_mr.baseColorFactor[0], pbr_mr.baseColorFactor[1], pbr_mr.baseColorFactor[2], pbr_mr.baseColorFactor[3]);
        curr_material.metallic_factor = pbr_mr.metallicFactor;
        curr_material.roughness_factor = pbr_mr.roughnessFactor;
        curr_material.normal_scale = material.normalTexture.scale;
        
        curr_mesh.material_index = primitive.material;
        
        if (g_material_cache.find(primitive.material) == g_material_cache.end()) {
            g_material_cache[primitive.material] = curr_material;
        } 
        
        output_meshes.push_back(curr_mesh);
    }

    return output_meshes;
}

static std::vector<gltf::MeshData>
load_meshes_from_node(const Root& root, int node_id, glm::mat4 parent_transform = glm::mat4(1.0f)) {
    /*- Node
        - Mesh
        - rotation - quaternion
        - matrix - mat4
        - scale - vec3
        - translation - vec3
        - Children[Node]
    */

    assert(node_id >= 0);
    const Node& curr_node = root.nodes[node_id];
    
    // Grab the transform information
    glm::mat4 local_transform = glm::mat4(1.0f);
    if (curr_node.matrix.size() == 16) {
        const std::vector<double>& m = curr_node.matrix;
        local_transform = glm::mat4{
            m[0],  m[1],  m[2],  m[3],
            m[4],  m[5],  m[6],  m[7],
            m[8],  m[9],  m[10], m[11],
            m[12], m[13], m[14], m[15]
        };
    } else {
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec3 translation = glm::vec3(0.0f);

        if (curr_node.rotation.size() == 4) {
            glm::quat qrot = glm::quat(curr_node.rotation[0], curr_node.rotation[1], curr_node.rotation[2], curr_node.rotation[3]);
            rotation = glm::mat4(qrot);
        }
        if (curr_node.scale.size() == 3) {
            scale = glm::vec3(curr_node.scale[0], curr_node.scale[1], curr_node.scale[2]);
        }
        if (curr_node.translation.size() == 3) {
            translation = glm::vec3(curr_node.translation[0], curr_node.translation[1], curr_node.translation[2]);
        }
        
        local_transform = rotation;
        local_transform = glm::scale(local_transform, scale);
        local_transform = glm::translate(local_transform, translation);
        assert(local_transform != glm::mat4(0.0f));
    }
    glm::mat4 curr_mesh_transform = parent_transform * local_transform;
    
    // Load the primitives in the current node's mesh
    std::vector<gltf::MeshData> output_meshes = {};
    if (curr_node.mesh >= 0) {
        output_meshes.reserve(curr_node.children.size() + root.meshes[curr_node.mesh].primitives.size());
        std::vector<gltf::MeshData> local_meshes = load_primitive_meshes(root, curr_node.mesh);
        for (auto& mesh : local_meshes) {
            mesh.transform = curr_mesh_transform;
            output_meshes.push_back(mesh);
        }
    } else {
        output_meshes.reserve(curr_node.children.size());
    }

    // Load the primitives from the current node's children
    for (auto& node_id: curr_node.children) {
        std::vector<gltf::MeshData> meshes = load_meshes_from_node(root, node_id, curr_mesh_transform);
        for (auto& mesh: meshes) {
            output_meshes.push_back(mesh);
        }
    }

    return output_meshes;
}

std::vector<gltf::ModelData> load_models_from_scene(const Root& root, int scene_id) {
    /*
        What does the hierachy look like for what we care for
        *Scene*
            - Nodes[]
                - Node
                    - Mesh
                    - rotation - quaternion
                    - matrix - mat4
                    - scale - vec3
                    - translation - vec3
                    - Children[Node]
                - Node
                ...
        *Mesh*
            - primitives[accessorIDs]
            - name
        
        *Mesh Primitive*
            - attributes {
                "POSITION": - accessorID (vec3f)
                "NORMAL": - accessorID (vec3f)
                "TANGENT": -accessorID (vec3f)
                "TEXCOORD_0": -accessorID (vec2) f?i8?u8?i16?u16?
                "TEXCOORD_1": -accessorID (vec2) f?i8?u8?i16?u16?
            }
            - indices  - accessorID (u32)
            - material - accessorID
            - mode : assumes GL_TRINAGLES
        *Material*
            - name
            - pbrMetallicRoughNess {
                baseColorFactor: vec4
                baseColorTexture: textureInfo
                metallicFactor: int
                roughnessFator: int
                metallicRoughnessTexture: textureInfo
            }
            - normalTexture {
                index: textureID
                texCoord: int
                scale: int
            }
        
        *TextureInfo*
            - index: textureID
            - texCoord: int

        *Accessor*
            - bufferView: bufferViewID
            - byteOffset: int
            - componentType: int
            - count: int
            - type: string
    */

    const Scene& scene = root.scenes[scene_id];
    std::vector<gltf::ModelData> output_models;
    output_models.reserve(scene.nodes.size());

    // Parsing top level nodes and turning them into model data
    for (auto& node_id : scene.nodes) {
        std::vector<gltf::MeshData> meshes = load_meshes_from_node(root, node_id, glm::mat4(1.0f));
        gltf::ModelData model = {};
        model.name = root.nodes[node_id].name;

        for (auto& mesh: meshes) {
            model.meshes.push_back(mesh);
        }

        output_models.push_back(model);
    }

    return output_models;
}

gltf::Scene gltf::load_gltf_file(const std::filesystem::path& path) {
    Root root;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&root, &err, &warn, path.string());

    if (!warn.empty()) {
        printf("WARNING: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("ERRROR: %s\n", err.c_str());
    }

    if (!ret) {
        printf("ERROR: Failed to parse glTF\n");
        return {};
    }

    // for each primitive in a mesh do:
    int main_scene_index = root.defaultScene;
    if (main_scene_index == -1) {
        return {};
    }

    g_base_asset_dir = path.parent_path().string();

    std::vector<gltf::ModelData> models = load_models_from_scene(root, main_scene_index);
    gltf::Scene output_scene = {};
    output_scene.materials = std::move(g_material_cache);
    output_scene.models = std::move(models);

    g_base_asset_dir = "";
    g_material_cache.clear();
    return output_scene;
}