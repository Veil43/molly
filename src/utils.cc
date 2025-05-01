#define RENDERTOY_HAS_GL
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <glad/glad.h>

#include <sstream>
#include <fstream>
#include <iostream>

#include "types.h"

std::string rdt::loadTextFile(const char* path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "UTIL::IO: Could not open file: " << path << std::endl;
        return "";
    }

    std::ostringstream text;
    text << file.rdbuf();
    return text.str();
}

void rdt::log(const std::string& message) {
    std::cerr << message << std::endl;
}

void rdt::printGLInfo() {
    GL_QUERY_ERROR(const char* version = (char*)glGetString(GL_VERSION);)
    GL_QUERY_ERROR(const char* vendor = (char*)glGetString(GL_VENDOR);)
    GL_QUERY_ERROR(const char* renderer = (char*)glGetString(GL_RENDERER);)
    GL_QUERY_ERROR(const char* glsl_version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);)
    rdt::log(
        std::string(
        "================== OpenGL Information ===================\n") + 
        "VERSION: " + version + "\n"
        "VENDOR: " + vendor + "\n"
        "RENDERER: " + renderer + "\n"
        "GLSL VERSION: " + glsl_version + "\n"
    );

    i32 max_texture_units = 0;
    i32 max_texture_size = 0;
    i32 max_draw_buffers = 0; // like color buffers, depth buffers etc
    i32 max_vertex_attributes = 0;
    i32 max_uniforms = 0;
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);)
    GL_QUERY_ERROR(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniforms);)

    rdt::log("[OpenGL::LIMITS]: Max Texture Units: " + std::to_string(max_texture_units));
    rdt::log("[OpenGL::LIMITS]: Max Texture Size: " + std::to_string(max_texture_size) + "x" + std::to_string(max_texture_size) + " Pixels");
    rdt::log("[OpenGL::LIMITS]: Max Draw Buffers: " + std::to_string(max_draw_buffers) + " (Like color/depth/stencil buffers)");
    rdt::log("[OpenGL::LIMITS]: Max Vertex Attributes: " + std::to_string(max_vertex_attributes));
    rdt::log("[OpenGL::LIMITS]: Max Uniforms: " + std::to_string(max_uniforms));

}

rdt::ImageData rdt::loadImageFile(const char* path) {
    rdt::ImageData result = {};
    int x = 0;
    int y = 0;
    int c = 0;
    unsigned char* data = stbi_load(path, &x, &y, &c, 0);
    if (!data) {
        rdt::log(std::string("ERROR::IO::STB_IMAGE: Could not load file with path <") + path + ">");
    } else {

        result.data = data;
        result.width = x;
        result.height = y;
        result.channel_count = c;
    }
    return result;
}

void rdt::freeImageData(ImageData* img) {
    if (!img || !img->data)
        return;
 
    stbi_image_free(img->data);
    img->data = nullptr;
    img->width = 0;
    img->height = 0;
    img->channel_count = 0;
}

// GLTF helper functions ---------------------------------------------------
namespace gltf =  tinygltf;
struct NodeTransformPair {
    tinygltf::Node node;
    glm::mat4 transform;
};

std::vector<NodeTransformPair> GLTFparseNodeTree(int head_id, const tinygltf::Model& model, const glm::mat4& parent_transform = glm::mat4(1.0f)) {
    using namespace tinygltf;
    
    Node curr_node = model.nodes[head_id];
    
    glm::mat4 local_transform = glm::mat4(1.0f);
    if (curr_node.matrix.size() == 16) {
        local_transform = glm::make_mat4(curr_node.matrix.data());
    } else {
        if (!curr_node.translation.empty()) {
            local_transform = glm::translate(local_transform, glm::vec3(
                curr_node.translation[0],
                curr_node.translation[1],
                curr_node.translation[2]
            ));
        }

        if (!curr_node.rotation.empty()) {
            glm::quat q(
                curr_node.rotation[0],
                curr_node.rotation[1],
                curr_node.rotation[2],
                curr_node.rotation[3]
            );
            local_transform *= glm::mat4_cast(q);
        }

        if (!curr_node.scale.empty()) {
            local_transform = glm::scale(local_transform, glm::vec3(
                curr_node.scale[0],
                curr_node.scale[1],
                curr_node.scale[2]
            ));
        }
    }

    glm::mat4 world_transform = parent_transform * local_transform;
    std::vector<NodeTransformPair> result;
    result.push_back({curr_node, world_transform});

    for (int child_id : curr_node.children) {
        auto children = GLTFparseNodeTree(child_id, model, world_transform);
        result.insert(result.end(), children.begin(), children.end());
    }

    return result;
}

struct GLTFDataCountPair {
    size_t count = 0;
    unsigned char* data;
};

GLTFDataCountPair GLTFGetAccessorData(tinygltf::Model* model, int accessor_index) {
    using namespace tinygltf;
    Accessor& accessor = model->accessors[accessor_index];
    BufferView& buffer_view = model->bufferViews[accessor.bufferView];
    Buffer& buffer = model->buffers[buffer_view.buffer];
    unsigned char* data = (&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

    return { accessor.count, data };
}

GLTFDataCountPair GLTFGetAttributeData(tinygltf::Primitive* primitive, tinygltf::Model* model, std::string attribute) {
    using namespace tinygltf;

    int accessor_index = primitive->attributes[attribute];

    return GLTFGetAccessorData(model, accessor_index);
}

rdt::eTextureConfigOptions GLTFToRDTConfigConvert(unsigned int gltf_macro) {
    switch (gltf_macro) {
        case TINYGLTF_TEXTURE_FILTER_NEAREST: {
            return rdt::eTextureConfigOptions::kTextureFilterNearest;
        } break;

        case TINYGLTF_TEXTURE_FILTER_LINEAR: {
            return rdt::eTextureConfigOptions::kTextureFilterLinear;
        } break;

        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST: {
            return rdt::eTextureConfigOptions::kTextureFilterNearestMipmapNearest;
        } break;

        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST: {
            return rdt::eTextureConfigOptions::kTextureFilterLinearMipmapNearest;
        } break;

        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR: {
            return rdt::eTextureConfigOptions::kTextureFilterNearestMipmapLinear;
        } break;

        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR: {
            return rdt::eTextureConfigOptions::kTextureFilterLinearMipmapLinear;
        } break;

        case TINYGLTF_TEXTURE_WRAP_REPEAT: {
            return rdt::eTextureConfigOptions::kTextureWrapRepeat;
        } break;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: {
            return rdt::eTextureConfigOptions::kTextureWrapClampToEdge;
        } break;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: {
            return rdt::eTextureConfigOptions::kTextureWrapMirroredRepeat;
        } break;

        default: {
            return rdt::eTextureConfigOptions::kError;
        }
    }
}

rdt::TextureInfo GLTFGetTextureData(gltf::Texture* texture, gltf::Model* model) {
    rdt::TextureInfo result = {};
    // check for negatives -1
    if (texture->sampler >= 0) {
        const gltf::Sampler& sampler = model->samplers[texture->sampler];
        result.filter_wrap_config.min_filter = GLTFToRDTConfigConvert(sampler.minFilter);
        result.filter_wrap_config.mag_filter = GLTFToRDTConfigConvert(sampler.magFilter);
        result.filter_wrap_config.wrap_s = GLTFToRDTConfigConvert(sampler.wrapS);
        result.filter_wrap_config.wrap_t = GLTFToRDTConfigConvert(sampler.wrapT);
    }

    if (texture->source >= 0) {
        const gltf::Image& image = model->images[texture->source];
        result.image_index = texture->source;
        result.image_uri = image.uri;
    }

    return result;
}

rdt::MaterialData GLTFGetPrimitiveMaterial(gltf::Primitive* primitive, gltf::Model* model) {
    rdt::MaterialData result = {};
    if (primitive->material < 0)  {
        return result;
    }
    // TODO: Get the material names
    const gltf::Material& primitive_material = model->materials[primitive->material];
    const gltf::PbrMetallicRoughness& pbr_component = primitive_material.pbrMetallicRoughness;
    const gltf::TextureInfo& base_color_info = pbr_component.baseColorTexture;
    const gltf::TextureInfo& metallic_roughness_info = pbr_component.metallicRoughnessTexture;
    const gltf::NormalTextureInfo& normal_info = primitive_material.normalTexture;
    const gltf::TextureInfo& emissive_texture = primitive_material.emissiveTexture;
    const gltf::OcclusionTextureInfo& occlusion_texture = primitive_material.occlusionTexture;
    
    result.base_color_factor = glm::vec4(
        pbr_component.baseColorFactor[0],
        pbr_component.baseColorFactor[1],
        pbr_component.baseColorFactor[2],
        pbr_component.baseColorFactor[3]
    );
    result.metallic_factor = static_cast<float>(pbr_component.metallicFactor);
    result.roughness_factor = static_cast<float>(pbr_component.roughnessFactor);
    

    if (base_color_info.index >= 0) {
        result.base_color = GLTFGetTextureData(&model->textures[base_color_info.index], model);
    }

    if (metallic_roughness_info.index >= 0) {
        result.metallic_roughness = GLTFGetTextureData(&model->textures[metallic_roughness_info.index], model);
    }

    if (normal_info.index >= 0) {
        result.normal = GLTFGetTextureData(&model->textures[normal_info.index], model);
    }

    // TODO: Add occlusion textures and emissive

    return result;
}

rdt::ModelData rdt::loadModel(const char* path) {
    gltf::Model model;
    gltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!warn.empty()) {
        rdt::log("WARNING: " + warn);
    }

    if (!err.empty()) {
        rdt::log("ERROR: " + err);
    }
    
    if (!ret) {
        std::string msg = "ERROR: Failed to load a 3D model from file: ";
        rdt::log(msg + path);
        return {};
    }

    ModelData final_model = {};
    // Transform the tinygltf model into one of our own
    // First grab all the image data
    for (gltf::Image& image : model.images) {
        ImageData image_data = {};
        image_data.data = new unsigned char[image.image.size()];
        image_data.channel_count = image.component;
        image_data.channel_size = image.bits;
        image_data.width = image.width;
        image_data.height = image.height;
        std::memcpy(image_data.data, image.image.data(), image.image.size());
        final_model.images.push_back(image_data);
    }

    gltf::Scene main_scene;
    if (model.defaultScene >= 0 && model.defaultScene < model.scenes.size()) {
        main_scene = model.scenes[model.defaultScene];
    } else {
        main_scene = model.scenes[0];
    }

    // TODO: Have a meshes array that stores all the UNIQUE meshes.
    // Currently we are treating each primitive as a standalone mesh, however it is possible that 
    // two distinct primitives reference the same accessors for position/normal/texcoord and therefore 
    // have the same geometry. In such a case we would want to have only one copy in an array and reference it with an index.
    // BASICALLY: do what we've done for images.

    for (auto& node_id : main_scene.nodes) {                            // parse top level nodes
        std::vector<NodeTransformPair> node_transform_pairs = GLTFparseNodeTree(node_id, model);

        for (auto& curr_node_pair : node_transform_pairs) {             // for each node in top level tree
            gltf::Node curr_node = curr_node_pair.node;

            if (curr_node.mesh < 0 || curr_node.mesh > model.meshes.size()) {
                continue;
            } 

            glm::mat4 node_transform = curr_node_pair.transform;

            gltf::Mesh node_mesh = model.meshes[curr_node.mesh];              // grab mesh data from each mesh
            for (auto& primitive : node_mesh.primitives) {              // for each primitive in the mesh
                rdt::MeshData mesh_data = {};
                rdt::MaterialData material_data = GLTFGetPrimitiveMaterial(&primitive, &model);
                
                mesh_data.world_transform = node_transform;

                GLTFDataCountPair positions_info = GLTFGetAttributeData(&primitive, &model, "POSITION");
                GLTFDataCountPair normals_info = GLTFGetAttributeData(&primitive, &model, "NORMAL");
                GLTFDataCountPair tex_coord_info = GLTFGetAttributeData(&primitive, &model, "TEXCOORD_0");
                // Get the primitive's material
            
                const unsigned int c_vertex_f32_count = 8;
                // Check for degenerate mesh
                if (positions_info.count != normals_info.count ||
                    normals_info.count != tex_coord_info.count)
                {
                    log("ERROR: Could not load model: Mismatched Vertex Attribute Count");
                    mesh_data.vertex_count = 0;
                    mesh_data.vertex_data = nullptr;
                } else {
                    mesh_data.vertex_count = positions_info.count;
                    mesh_data.vertex_data = new float[mesh_data.vertex_count * c_vertex_f32_count];
                    for (int i = 0; i < mesh_data.vertex_count; i++) {
                        float* dst = mesh_data.vertex_data + i * c_vertex_f32_count;

                        const float* position = reinterpret_cast<float*>(positions_info.data) + i * 3;
                        const float* normal = reinterpret_cast<float*>(normals_info.data) + i * 3;
                        const float* tex_coord = reinterpret_cast<float*>(tex_coord_info.data) + i * 2;

                        std::memcpy(dst,        position, sizeof(float) * 3);
                        std::memcpy(dst + 3,    normal, sizeof(float) * 3);
                        std::memcpy(dst + 6,    tex_coord, sizeof(float) * 2);
                    }
                }

                // get indices (Check accessor component type)
                if (primitive.indices >= 0) {
                    const gltf::Accessor& accessor = model.accessors[primitive.indices];
                    GLTFDataCountPair indices_info = GLTFGetAccessorData(&model, primitive.indices);
                    mesh_data.index_data = new unsigned int[indices_info.count];
                    mesh_data.index_count = indices_info.count;

                    switch (accessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                            const unsigned int* src = reinterpret_cast<const unsigned int*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = src[i];
                            }
                        } break;

                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                            const unsigned char* src = reinterpret_cast<const unsigned char*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = static_cast<unsigned int>(src[i]);
                            }
                        } break;

                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            const unsigned short* src = reinterpret_cast<const unsigned short*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = static_cast<unsigned int>(src[i]);
                            }
                        } break;

                        case TINYGLTF_COMPONENT_TYPE_INT: {
                            const int* src = reinterpret_cast<const int*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = static_cast<unsigned int>(src[i]);
                            }
                        } break;

                        case TINYGLTF_COMPONENT_TYPE_BYTE: {
                            const char* src = reinterpret_cast<const char*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = static_cast<unsigned int>(src[i]);
                            }
                        } break;

                        case TINYGLTF_COMPONENT_TYPE_SHORT: {
                            const short* src = reinterpret_cast<const short*>(indices_info.data);
                            for (int i = 0; i < indices_info.count; i++) {
                                mesh_data.index_data[i] = static_cast<unsigned int>(src[i]);
                            }
                        } break;

                        default : {
                            mesh_data.index_data = nullptr;
                            mesh_data.index_count = 0;
                            log("ERROR: Some joker thought it would be funny to use floats for indices. No element buffer for you!");
                        }
                    }

                } else {
                    mesh_data.index_data = nullptr;
                    mesh_data.index_count = 0;
                }

                final_model.materials.push_back(material_data);
                mesh_data.material_index = final_model.materials.size() - 1;
                final_model.meshes.push_back(mesh_data);
            }
        }
    }

    return final_model;
}