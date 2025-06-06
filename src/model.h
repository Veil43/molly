#ifndef RENDERTO_MODEL_H
#define RENDERTO_MODEL_H

#include "types.h"
#include "molly_math.h"
#include "shader.h"
#include "gltf_loader.h"

static const u32 kPositionIndex = 0;
static const u32 kNormalIndex = 1;
static const u32 kTexCoordIndex = 2;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

// -----------------------------------------
// glTF
// -----------------------------------------
struct MeshHandle {
    glm::mat4 transform;
    u32 vbo[3];
    u32 vao;
    u32 ebo;
    u32 indices_type;
    int icount;
    int vcount;
    int material_index;
};

struct ModelHandle {
    std::vector<MeshHandle> meshes;
};

struct TextureHandle {
    u32 texture_unit;
    u32 texture;
};

struct MaterialHandle {
    TextureHandle diffuse;
    TextureHandle metallic_roughness;
    TextureHandle normal;
};

struct SceneHandle {
    std::vector<ModelHandle> models;
    std::map<int, MaterialHandle> materials;
};

MeshHandle load_gltf_mesh_to_opengl(gltf::MeshData& mesh, bool interleave = false);
ModelHandle load_gltf_model_to_opengl(gltf::ModelData& model, bool interleave);
MaterialHandle load_gltf_material_to_opengl(gltf::MaterialInfo& material);
SceneHandle load_gltf_scene_to_opengl(gltf::SceneData& scene, bool interleave);

void draw_gltf_model(const ModelHandle& m, Shader& shader);
void draw_gltf_scene(const SceneHandle& scene, Shader& shader);

#endif // RENDERTO_MODEL_H