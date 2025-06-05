#define MOLLY_HAS_GL
#include <glad/glad.h>

#include <cassert>

#include "model.h"
#include "utils.h"
#include "logger.h"

/// TODO: please figure out a more robust way to handle texture counts than this.
static int g_texture_counter = 0;

// -----------------------------------------------------------
// Model loading
// -----------------------------------------------------------
gMeshHandle load_gltf_mesh_to_opengl(gltf::MeshData& mesh, bool interleave) {
    gMeshHandle output_mesh = {};
    output_mesh.transform = mesh.transform;
    output_mesh.icount = mesh.indices_data.size();
    output_mesh.material_index = mesh.material_index;
    GL_QUERY_ERROR(glGenVertexArrays(1, &output_mesh.vao);)
    GL_QUERY_ERROR(glBindVertexArray(output_mesh.vao);)
    GL_QUERY_ERROR(glGenBuffers(3, output_mesh.vbo);)
    // --------------- Position Loading -----------------------
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, output_mesh.vbo[kPositionIndex]);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, mesh.pos_data.size(), mesh.pos_data.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(kPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kPositionIndex);)
    // --------------- Normal Loadiing ------------------------
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, output_mesh.vbo[kNormalIndex]);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, mesh.nor_data.size(), mesh.nor_data.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(kNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kNormalIndex);)
    // -------------- Texture Coord Loading -------------------
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, output_mesh.vbo[kTexCoordIndex]);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, mesh.tex_data.size(), mesh.tex_data.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(kTexCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kTexCoordIndex);)
    // -------------- Indices Loading -------------------------
    GL_QUERY_ERROR(glGenBuffers(1, &output_mesh.ebo);)
    GL_QUERY_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, output_mesh.ebo);)
    GL_QUERY_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices_data.size(), mesh.indices_data.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glBindVertexArray(0);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0);)
    GL_QUERY_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);)

    return output_mesh;
}

gModelHandle load_gltf_model_to_opengl(gltf::ModelData& model, bool interleave) {
    gModelHandle output_model = {};
    for (auto& m : model.meshes) {
        gMeshHandle output_mesh = load_gltf_mesh_to_opengl(m, interleave);
        output_model.meshes.push_back(output_mesh);
    }

    return output_model;
}

// ---------------------------------------------------------------------
// Material Loading
// ---------------------------------------------------------------------
gTextureHandle load_gltf_texture_to_opengl(gltf::TextureInfo& texture) {
    gTextureHandle output_handle = {};

    utils::ImageData image = utils::load_image_file(utils::resolve_path(texture.path), false);
    
    if (image.data == nullptr) {
        logger::log_debug(("could not find the path: ") + texture.path, logger::eLoggingLevel::kError, 100);
        return output_handle;
    } else {
        logger::log_debug(("found path: ") + texture.path, logger::eLoggingLevel::kWarning, 100);
    }

    u32 internal_format = GL_RGBA8;
    if (image.channel_count == 3) {
        internal_format = GL_RGB8;
    }

    GL_QUERY_ERROR(glGenTextures(1, &output_handle.texture);)
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + g_texture_counter);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, output_handle.texture);)
    
    GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);)
    GL_QUERY_ERROR(glGenerateMipmap(GL_TEXTURE_2D);)

    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);)

    output_handle.texture_unit = g_texture_counter;
    g_texture_counter++;
    return output_handle;
}

gMaterialHandle load_gltf_material_to_opengl(gltf::MaterialInfo& material) {
    gMaterialHandle output_material = {};
    output_material.diffuse = load_gltf_texture_to_opengl(material.diffuse_map);
    output_material.metallic_roughness = load_gltf_texture_to_opengl(material.metallic_roughness_map);
    output_material.normal = load_gltf_texture_to_opengl(material.normal_map);
    return output_material;
}

// ---------------------------------------------------------------------
// Scene Loading
// ---------------------------------------------------------------------
gSceneHandle load_gltf_scene_to_opengl(gltf::Scene& s, bool interleave) {
    gSceneHandle output_scene = {};
    output_scene.models.reserve(s.models.size());
    for (auto& m : s.models) {
        output_scene.models.push_back(load_gltf_model_to_opengl(m, interleave));
    }

    /// TODO: see if pre-allocating makes any difference here 
    // (we won't be having HUGE numbers of materials)
    for (auto& m : s.materials) {
        output_scene.materials[m.first] = load_gltf_material_to_opengl(m.second);
    }

    return output_scene;
}

// ---------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------
void draw_gltf_model(const gModelHandle& model, const gSceneHandle& scene, Shader& shader) {
    shader.bind();
    for (auto& mesh : model.meshes) {
        const gMaterialHandle& material = scene.materials.at(mesh.material_index);
        shader.set_mat4f("model", mesh.transform);
        shader.set_int("diffuse_map", material.diffuse.texture_unit);
        shader.set_int("metallic_roughness_map", material.metallic_roughness.texture_unit);
        shader.set_int("normal_map", material.normal.texture_unit);
        GL_QUERY_ERROR(glBindVertexArray(mesh.vao);)
        GL_QUERY_ERROR(glDrawElements(GL_TRIANGLES, mesh.icount, GL_UNSIGNED_INT, 0);)
        GL_QUERY_ERROR(glBindVertexArray(0);)
    }
    shader.unbind();    
}

void draw_gltf_scene(const gSceneHandle& scene, Shader& shader) {
    for (const auto& m : scene.models) {
        draw_gltf_model(m, scene, shader);
    }
}