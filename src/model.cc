#define MOLLY_HAS_GL
#include <glad/glad.h>

#include <cassert>

#include "model.h"
#include "utils.h"
#include "logger.h"
#include "shader.h"

/// TODO: please figure out a more robust way to handle texture counts than this.
static int g_texture_counter = 0;

// -----------------------------------------------------------
// Model loading
// -----------------------------------------------------------
MeshHandle load_gltf_mesh_to_opengl(gltf::MeshData& mesh, bool interleave) {
    MeshHandle output_mesh = {};
    output_mesh.transform = mesh.transform;
    output_mesh.icount = mesh.indices_data.size();
    output_mesh.vcount = mesh.pos_data.size() / 8;      /// NOTE: to change with more attributes
    output_mesh.material_index = mesh.material_index;
    output_mesh.indices_type = GL_UNSIGNED_INT;

    // see the glTF spec 5.24.2 mesh.primitive.indices: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_mesh_primitive_indices
    switch(mesh.indices_type) {
        case gltf::eBasicType::kByte:             { output_mesh.indices_type = GL_BYTE; } break;
        case gltf::eBasicType::kUnsignedByte:     { output_mesh.indices_type = GL_UNSIGNED_BYTE; } break;
        case gltf::eBasicType::kShort:            { output_mesh.indices_type = GL_SHORT; } break;
        case gltf::eBasicType::kUnsignedShort:    { output_mesh.indices_type = GL_UNSIGNED_SHORT; } break;
        case gltf::eBasicType::kUnsignedInt:      { output_mesh.indices_type = GL_UNSIGNED_INT; } break;
        case gltf::eBasicType::kFloat:            { 
            logger::log_debug("!!critical!! indices were passed in as float. runtime crash likely.",
                              logger::eLoggingLevel::kError, 100);

            output_mesh.indices_type = GL_FLOAT; 
        } break;
    }

    // see the glTF spec 3.7.2. Meshes: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#geometry
    u32 texture_type = GL_FLOAT;
    if (mesh.texture_type_0 == gltf::eBasicType::kUnsignedByte) {
        texture_type = GL_UNSIGNED_BYTE;
    } else if (mesh.texture_type_0 == gltf::eBasicType::kUnsignedShort) {
        texture_type = GL_UNSIGNED_SHORT;
    }

    /// TODO: check what happens when buffer.data() == nullptr and buffer.size() == 0;

    GL_QUERY_ERROR(glGenVertexArrays(1, &output_mesh.vao);)
    GL_QUERY_ERROR(glBindVertexArray(output_mesh.vao);)
    GL_QUERY_ERROR(glGenBuffers(kAttributeCount, output_mesh.vbo);)
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
    // --------------- Tangent Loading ------------------------
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, output_mesh.vbo[kTangentIndex]);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, mesh.tan_data.size(), mesh.tan_data.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(kTangentIndex, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kTangentIndex);)
    // -------------- Texture Coord Loading -------------------
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, output_mesh.vbo[kTexCoordIndex]);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, mesh.tex_data_0.size(), mesh.tex_data_0.data(), GL_STATIC_DRAW);)
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

ModelHandle load_gltf_model_to_opengl(gltf::ModelData& model, bool interleave) {
    ModelHandle output_model = {};
    for (auto& m : model.meshes) {
        MeshHandle output_mesh = load_gltf_mesh_to_opengl(m, interleave);
        output_model.meshes.push_back(output_mesh);
    }

    return output_model;
}

// ---------------------------------------------------------------------
// Material Loading
// ---------------------------------------------------------------------
void bind_texture_to_unit(u32 texture, u32 unit) {
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + unit);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture);)
}

TextureHandle load_gltf_texture_to_opengl(gltf::TextureInfo& texture, const std::string& name = "none") {
    TextureHandle output_handle = {};

    utils::ImageData image = utils::load_image_file(utils::resolve_path(texture.path), false);

    if (image.data == nullptr) {
        logger::log_debug(name, logger::eLoggingLevel::kWarning, 20.0f);
        image = utils::load_image_file(utils::resolve_path("assets/textures/errorP6.ppm"));
        if (image.data == nullptr) {
            utils::cmdlog("ERROR: Could not locate the fallback texture after texture loading failed");
            exit(1);
        }
    }

    u32 internal_format = GL_RGBA8;
    if (image.channel_count == 3) {
        internal_format = GL_RGB8;
    }
    u32 format = GL_RGBA;
    if (image.channel_count == 3) {
        format = GL_RGB;
    }

    GL_QUERY_ERROR(glGenTextures(1, &output_handle.texture);)
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + g_texture_counter);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, output_handle.texture);)
    
    GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);)
    GL_QUERY_ERROR(glGenerateMipmap(GL_TEXTURE_2D);)

    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);)

    output_handle.texture_unit = g_texture_counter;
    g_texture_counter++;
    return output_handle;
}

MaterialHandle load_gltf_material_to_opengl(gltf::MaterialInfo& material, const std::string& shader_name) {
    MaterialHandle output_material = {};
    
    output_material.diffuse = load_gltf_texture_to_opengl(material.diffuse_map, material.name + " diffuse");
    output_material.metallic_roughness = load_gltf_texture_to_opengl(material.metallic_roughness_map, material.name + " metallic_roughness");
    output_material.normal = load_gltf_texture_to_opengl(material.normal_map, material.name + " normal");
    output_material.shader_name = shader_name;

    output_material.diffuse_factor = material.diffuse_factor;
    output_material.metallic_factor = material.metallic_factor;
    output_material.roughness_factor = material.roughness_factor;

    return output_material;
}

// ---------------------------------------------------------------------
// Scene Loading
// ---------------------------------------------------------------------
SceneHandle load_gltf_scene_to_opengl(gltf::SceneData& scene, bool interleave) {
    SceneHandle output_scene = {};
    output_scene.models.reserve(scene.models.size());
    for (auto& m : scene.models) {
        output_scene.models.push_back(load_gltf_model_to_opengl(m, interleave));
    }

    /// TODO: see if pre-allocating makes any difference here 
    // (we won't be having HUGE numbers of materials)
    logger::log_debug(std::to_string(scene.materials.size()) + " materials in scene", 
                      logger::eLoggingLevel::kWarning,
                      20.0f);

    output_scene.materials.reserve(scene.materials.size());
    for (auto& m : scene.materials) {
        MaterialHandle gl_material = load_gltf_material_to_opengl(m);
        output_scene.materials.push_back(gl_material);
    }

    return output_scene;
}

// ---------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------
void draw_gltf_model(const ModelHandle& model, const SceneHandle& scene, Camera& camera, glm::vec3& light1_position, Transform& scene_transform) {
    for (auto& mesh : model.meshes) {
        const MaterialHandle& material = scene.materials.at(mesh.material_index);

        Shader shader = request_shader_of_name(material.shader_name);

        shader.bind();
        Transform object_transform = compose(scene_transform, mesh.transform);
        glm::mat4 model = object_transform.to_mat4();

        shader.set_mat4f("model", model);
        shader.set_mat4f("view", camera.get_view_matrix());
        shader.set_mat4f("projection", camera.get_projection_matrix());

        shader.set_vec3f("point_light1_position", light1_position);
        shader.set_vec3f("camera_position", camera.m_position);

        shader.set_int("diffuse_map", material.diffuse.texture_unit);
        shader.set_int("metallic_roughness_map", material.metallic_roughness.texture_unit);
        shader.set_int("normal_map", material.normal.texture_unit);

        shader.set_vec4f("diffuse_factor", material.diffuse_factor);
        shader.set_float("metallic_factor", material.metallic_factor);
        shader.set_float("roughness_factor", material.roughness_factor);

        GL_QUERY_ERROR(glBindVertexArray(mesh.vao);)
        if (mesh.icount > 0) {
            GL_QUERY_ERROR(glDrawElements(GL_TRIANGLES, mesh.icount, mesh.indices_type, 0);)
        } else {
            GL_QUERY_ERROR(glDrawArrays(GL_TRIANGLES, 0, mesh.vcount);)
        }
        GL_QUERY_ERROR(glBindVertexArray(0);)
        shader.unbind();    
    }
}

void draw_gltf_scene(const SceneHandle& scene, Camera& camera, glm::vec3& light1_position, Transform& parent_transform) {
    for (const auto& m : scene.models) {
        draw_gltf_model(m, scene, camera, light1_position, parent_transform);
    }
}

//
void draw_molly_scene(Scene& scene) {
    Transform transform = scene.transform;
    draw_gltf_scene(scene.handle, scene.camera, scene.light1.position, transform);
}
