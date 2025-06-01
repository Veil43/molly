#ifndef RENDERTO_MODEL_H
#define RENDERTO_MODEL_H

#include "types.h"
#include "molly_math.h"
#include "utils.h"
#include "shader.h"

#include <string>

static const u32 kPositionIndex = 0;
static const u32 kNormalIndex = 1;
static const u32 kTexCoordIndex = 2;

static const u32 kDiffuseTexture = 0;
static const u32 kNormalTexture = 1;
static const u32 kSpecularTexture = 2;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

struct MaterialData {
    std::string diffuse;
    std::string normal;
    std::string specular;
};

struct ModelData {
    std::vector<Vertex> vertices;
    std::vector<i32> indices;

    i32 vcount;
    i32 icount;

    MaterialData material;
};

struct MaterialHandle {
    molly::ImageData diff_image;
    molly::ImageData norm_image;
    molly::ImageData spec_image;

    u32 diff_texture;
    u32 norm_texture;
    u32 spec_texture;
};

struct ModelHandle {
    u32 vao;
    u32 vbo;
    u32 ebo;
    int vcount;
    int icount;
};

ModelData load_model_obj(const std::string& path);
ModelHandle load_model_to_opengl(ModelData& m);
MaterialHandle load_material_to_opengl(MaterialData& m);
void draw_model(ModelHandle& m, Shader& shader);

#endif // RENDERTO_MODEL_H