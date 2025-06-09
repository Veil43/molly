#ifndef RENDERTO_MODEL_H
#define RENDERTO_MODEL_H

#include "types.h"
#include "molly_math.h"
#include "gltf_loader.h"
#include "camera.h"

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
    // glm::vec3 position;
    // glm::vec3 scale;
    // glm::quat rotation;
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
    std::string shader_name = "mr_phong";
};

struct SceneHandle {
    std::vector<ModelHandle> models;
    std::vector<MaterialHandle> materials;
};

MeshHandle load_gltf_mesh_to_opengl(gltf::MeshData& mesh, bool interleave = false);
ModelHandle load_gltf_model_to_opengl(gltf::ModelData& model, bool interleave);
MaterialHandle load_gltf_material_to_opengl(gltf::MaterialInfo& material);
SceneHandle load_gltf_scene_to_opengl(gltf::SceneData& scene, bool interleave);

// void draw_gltf_model(const ModelHandle& m, const SceneHandle& scene);
// void draw_gltf_scene(const SceneHandle& scene, Camera& camera);

// ----------------------------
// Scene stuff
// ----------------------------
struct Light {
    enum class eLightType {
        kPoint,
        kDirectional,
        kSpot,
    };

    eLightType type;
    glm::vec3 direction;
    glm::vec3 position;
};

struct Scene {
    SceneHandle handle;
    Camera camera;
    Light light1; // simple point light
    Light light2;
    Light light3;
};

void draw_molly_scene(Scene& scene);

#endif // RENDERTO_MODEL_H