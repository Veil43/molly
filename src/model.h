#ifndef RENDERTO_MODEL_H
#define RENDERTO_MODEL_H

#include "types.h"
#include "molly_math.h"
#include "utils.h"
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
struct gMeshHandle {
    u32 vao;
    u32 vbo[3];
    u32 ebo;
    glm::mat4 transform;
    int icount;
    int material_index;
};

struct gModelHandle {
    std::vector<gMeshHandle> meshes;
};

struct gTextureHandle {
    u32 texture_unit;
    u32 texture;
};

struct gMaterialHandle {
    gTextureHandle diffuse;
    gTextureHandle metallic_roughness;
    gTextureHandle normal;
};

struct gSceneHandle {
    std::vector<gModelHandle> models;
    std::map<int, gMaterialHandle> materials;
};

gMeshHandle load_gltf_mesh_to_opengl(gltf::MeshData& mesh, bool interleave = false);
gModelHandle load_gltf_model_to_opengl(gltf::ModelData& model, bool interleave);
gMaterialHandle load_gltf_material_to_opengl(gltf::MaterialInfo& material);
gSceneHandle load_gltf_scene_to_opengl(gltf::Scene& scene, bool interleave);

void draw_gltf_model(const gModelHandle& m, Shader& shader);
void draw_gltf_scene(const gSceneHandle& scene, Shader& shader);

#endif // RENDERTO_MODEL_H