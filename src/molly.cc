#define MOLLY_HAS_GL

#include "molly.h"
#include <glad/glad.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#include "platform_services.h"

#include "shader.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "molly_math.h"
#include "camera.h"
#include "material.h"
#include "light.h"
// #include "context.h"

static f32 vertices[] = {
     0.5f,  0.5f, 0.0f,    1.0f, 1.0f,    1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,    0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f, 1.0f,
};

static u32 indices[] = {
    0, 1, 2,
    0, 2, 3
};

static f32 square_vertices[] = {
     // positions           // normals           // uvs
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,   0.0f, 0.0f,
    
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
};

static const u32 kPositionIndex = 0;
static const u32 kNormalIndex = 1;
static const u32 kTexCoordIndex = 2;

static u32 basic_shader_id = 0;
static u32 triangle_vao = 0;

/*
    DO NOT USE ME!
    I BROKEY!!
*/
static void sampleOpenGLProgram() {
    // Data ----------
    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    u32 vbo;
    u32 ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (void*)vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (void*)indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // TODO: Shader ----------
    const char* vertex_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "layout (location = 2) in vec3 aCol;\n"
    "\n"
    "out vec3 vertex_color;\n"
    "out vec2 tex_coord;\n"
    "\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   vertex_color = aCol;\n"
    "   tex_coord = aTexCoord;\n"
    "}\n"
    "";

    const char* fragment_source =
    "#version 330 core\n"
    "out vec4 frag_color;\n"
    "\n"
    "in vec3 vertex_color;\n"
    "in vec2 tex_coord;\n"
    "\n"
    "uniform sampler2D texture1;"
    "\n"
    "void main() {\n"
    "   frag_color = vec4(vertex_color, 1.0f) * texture(texture1, tex_coord);\n"
    "}\n"
    "";

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);

    glCompileShader(vertex_shader);
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        // TODO: Have a printing function that prints things to the screen
        std::cerr << "ERROR::SHADER::VERTEX::FAILED_TO_COMPILE: " 
        << info_log << std::endl;
    }

    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        // TODO: Have a printing function that prints things to the screen
        std::cerr << "ERROR::SHADER::FRAGMENT::FAILED_TO_COMPILE: " 
        << info_log << std::endl;
    }

    u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        // TODO: Have a printing function that prints things to the screen
        std::cerr << "ERROR::SHADER::FAILED_TO_COPILE: " 
        << info_log << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    triangle_vao = vao;
    basic_shader_id = shader_program;
    
    // Texture ----------
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load data 
    // if data do else NO!
    std::string texture_path = "../data/container.png";
    int width, height, channel_count;
    u8* data;
    data = nullptr;
#ifdef STBI_INCLUDE_STB_IMAGE_H
    data = stbi_load(texture_path.c_str(), &width, &height, &channel_count, 0);
#endif
    if (!data) {
        std::cerr << "ERROR::IO::STB: Could not load file: " << texture_path; 
    } else  {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    u32 texture_unit = 0;
    glUseProgram(basic_shader_id);
    glUniform1i(glGetUniformLocation(basic_shader_id, "texture1"), texture_unit);
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(0);
}

static StaticMesh cube{};
static std::vector<StaticMesh> backpack{};

static glm::vec3 cube_position(0.0f, 0.0f, -4.0f);
static Shader shader{};
static Texture container_diffuse_map{};
static Texture container_specular_map{};
static Camera cam{};
static u32* texture_handles;
static std::vector<molly::MaterialData> s_materials;

u32 convertTextureConfigOption(u32 option) {
    switch (static_cast<molly::eTextureConfigOptions>(option)) {
        case molly::eTextureConfigOptions::kTextureFilterNearest: {
            return GL_NEAREST;
        } break;
        case molly::eTextureConfigOptions::kTextureFilterLinear: {
            return GL_LINEAR;
        } break;
        case molly::eTextureConfigOptions::kTextureFilterNearestMipmapNearest: {
            return GL_NEAREST_MIPMAP_NEAREST;
        } break;
        case molly::eTextureConfigOptions::kTextureFilterLinearMipmapNearest: {
            return GL_LINEAR_MIPMAP_NEAREST;
        } break;
        case molly::eTextureConfigOptions::kTextureFilterNearestMipmapLinear: {
            return GL_NEAREST_MIPMAP_LINEAR;
        } break;
        case molly::eTextureConfigOptions::kTextureFilterLinearMipmapLinear: {
            return GL_LINEAR_MIPMAP_LINEAR;
        } break;
        case molly::eTextureConfigOptions::kTextureWrapRepeat: {
            return GL_REPEAT;
        } break;
        case molly::eTextureConfigOptions::kTextureWrapClampToEdge: {
            return GL_CLAMP_TO_EDGE;
        } break;
        case molly::eTextureConfigOptions::kTextureWrapMirroredRepeat: {
            return GL_MIRRORED_REPEAT;
        } break;
        case molly::eTextureConfigOptions::kError: {
            molly::log("ERROR::TEXTURE_CONFIG::CONVERSION: Invalid texure config");
            return 0;
        } break;

        default: {
            molly::log("ERROR???::TEXTURE_CONFIG::CONVERSION: Invalid texure config");
            return 0;
        }
    }
}

void configureTexture(const molly::TextureConfig& config) {
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convertTextureConfigOption(config.min_filter));)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convertTextureConfigOption(config.mag_filter));)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convertTextureConfigOption(config.wrap_s));)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convertTextureConfigOption(config.wrap_t));)
}

// to be called at runtime (I think)
void setMaterial(const Shader& shader, const molly::MaterialData& mat) {
    // struct MaterialData {
    //     TextureInfo base_color;
    //     TextureInfo metallic_roughness;
    //     TextureInfo normal;
    //     // Currently not supported
    //     TextureInfo emissive; 
    //     // Currently not supported
    //     TextureInfo occlusion;
    //     glm::vec4 base_color_factor;
    //     float metallic_factor;
    //     float roughness_factor;
    // };
    // #define MOLLY_DIFFUSE_MAP_TEXTURE_UNIT            (0)
    // #define MOLLY_SPECULAR_MAP_TEXTURE_UNIT           (1)
    // #define MOLLY_METALLIC_ROUGHNESS_MAP_TEXTURE_UNIT (2)
    // #define MOLLY_NORMAL_MAP_TEXTURE_UNIT             (3)

    // shader.bind();
    // -------------- vecn + scalar values -----------------
    shader.setVec4f("pbr_material.diffuse_color_factor", mat.base_color_factor);
    shader.setFloat("pbr_material.metallic_factor", mat.metallic_factor);
    shader.setFloat("pbr_material.roughness_factor", mat.roughness_factor);
    // --------------- sampler2D values -----------------
    // --------------------- DIFFUSE ----------------------
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + MOLLY_DIFFUSE_MAP_TEXTURE_UNIT);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture_handles[mat.base_color.image_index]);)
    configureTexture(mat.base_color.filter_wrap_config);
    shader.setInt("pbr_material.diffuse_map", MOLLY_DIFFUSE_MAP_TEXTURE_UNIT);

    // ----------------- METALLIC-ROUGHNESS ------------------
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + MOLLY_METALLIC_ROUGHNESS_MAP_TEXTURE_UNIT);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture_handles[mat.metallic_roughness.image_index]);)
    configureTexture(mat.metallic_roughness.filter_wrap_config);
    shader.setInt("pbr_material.metallic_roughness_map", MOLLY_METALLIC_ROUGHNESS_MAP_TEXTURE_UNIT);
    
    // ------------------ NORMAL ----------------------------
    GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + MOLLY_NORMAL_MAP_TEXTURE_UNIT);)
    GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture_handles[mat.normal.image_index]);)
    configureTexture(mat.normal.filter_wrap_config);
    shader.setInt("pbr_material.normal_map", MOLLY_NORMAL_MAP_TEXTURE_UNIT);
    // shader.setInt("pbr_material.emissive_map", mat.normal.image_index);
    // shader.setInt("pbr_material.occlusion_map", mat.normal.image_index);
    // shader.unbind();
}

void mollyToyOnStartupCall(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    // -- Platform Configurations --
    platformDisableMouseCursor();

    // TODO: Have an asset manager
    // We will need a globally available context for the renderer
    // g_context.asset_registry;

    molly::ModelData backpack_model = molly::loadModel("../data/survival_guitar_backpack/scene.gltf");
    texture_handles = new u32[backpack_model.images.size()];
    glGenTextures(backpack_model.images.size(), texture_handles);
    s_materials = backpack_model.materials;

    // Load texture images to GPU and store handles sequentially
    for (size_t i = 0; i < backpack_model.images.size(); i++) {
        GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, texture_handles[i]);)

        // TODO: Verify correctness
        u32 format = GL_RGB + (backpack_model.images[i].channel_count % 3);
        u32 internal_format = GL_RGB8;
        if (backpack_model.images[i].channel_size == 16) {
            internal_format = GL_RGB16;
        }
        GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, backpack_model.images[i].width, backpack_model.images[i].height, 0, format, GL_UNSIGNED_BYTE, backpack_model.images[i].data);)
        GL_QUERY_ERROR(glGenerateMipmap(GL_TEXTURE_2D);)
    }

    for (auto& mesh : backpack_model.meshes) {
        StaticMesh static_mesh{mesh};
        // Where is this meshe's material?
        const molly::MaterialData& mesh_mat = backpack_model.materials[mesh.material_index]; 
        backpack.push_back(std::move(static_mesh));
    }

    molly::printGLInfo();
    cube = StaticMesh((Vertex*)square_vertices, nullptr, 36, 0);
    // shader = Shader("../src/shaders/vsample.glsl", "../src/shaders/fsample.glsl");
    shader = Shader("../src/shaders/vno_mat.glsl", "../src/shaders/fno_mat.glsl");
    container_diffuse_map = Texture("../data/container.png");
    container_specular_map= Texture("../data/container_specular.png");

    shader.bind();
    shader.setInt("container.diffuse_map", MOLLY_DIFFUSE_MAP_TEXTURE_UNIT);
    shader.setInt("container.specular_map", MOLLY_SPECULAR_MAP_TEXTURE_UNIT);
    shader.setFloat("container.shininess", 32.0f);
    container_diffuse_map.bind(MOLLY_DIFFUSE_MAP_TEXTURE_UNIT);
    container_specular_map.bind(MOLLY_SPECULAR_MAP_TEXTURE_UNIT);
    shader.unbind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, cube_position);
    // model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f));
    
    Camera tmp(glm::vec3(0.0f, 0.0f, 500.0f));
    cam = tmp;
    cam.m_far = 5000.0f;
    cam.m_movement_speed = 100.0f;

    cam.m_aspect_ratio = aspect_ratio;
#ifdef GIMP
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    PointLight point_light1 = {};
    point_light1.position = glm::vec3(1.0f);
    point_light1.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    point_light1.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    point_light1.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    point_light1.specular = glm::vec3(1.0f);

    SpotLight spot_light1 = {};
    spot_light1.position = glm::vec3(0.0f, 0.0f, 0.4f);
    spot_light1.direction = -spot_light1.position;
    spot_light1.attenuation = glm::vec3(1.0f, 0.009f, 0.0075f);
    spot_light1.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    spot_light1.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    spot_light1.specular = glm::vec3(1.0f);
    spot_light1.cutoff = glm::cos(glm::radians(12.5f));
    spot_light1.outer_cutoff = glm::cos(glm::radians(17.5f));

    shader.bind();
    shader.setVec3f("camera_position", cam.m_position);

    shader.setMat4f("model", model);
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);
    shader.setVec3f("point_light1.position", point_light1.position);
    shader.setVec3f("point_light1.attenuation", point_light1.position);
    shader.setVec3f("point_light1.ambient", point_light1.ambient);
    shader.setVec3f("point_light1.diffuse", point_light1.diffuse);
    shader.setVec3f("point_light1.specular", point_light1.specular);

    shader.setVec3f("spot_light1.position", spot_light1.position);
    shader.setVec3f("spot_light1.direction", spot_light1.direction);
    shader.setVec3f("spot_light1.attenuation", spot_light1.attenuation);
    shader.setVec3f("spot_light1.ambient", spot_light1.ambient);
    shader.setVec3f("spot_light1.diffuse", spot_light1.diffuse);
    shader.setVec3f("spot_light1.specular", spot_light1.specular);
    shader.setFloat("spot_light1.cutoff", spot_light1.cutoff);
    shader.setFloat("spot_light1.outer_cutoff", spot_light1.outer_cutoff);
    shader.unbind();
#endif
}

std::vector<glm::vec3> container_positions = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3( 2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f, 2.0f, -2.5f),
    glm::vec3( 1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

void renderBackPack() {
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();
    
    shader.bind();
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);    

    for (auto& mesh : backpack) {
        glm::mat4 model = mesh.m_transform;
        const molly::MaterialData& mesh_mat = s_materials[mesh.m_material_index]; 
        
        setMaterial(shader, mesh_mat);
        mesh.bind();
        shader.setMat4f("model", model);
        mesh.draw();
        mesh.unbind();
    }
    shader.unbind();
}

void mollyToyRenderLoop(f64 delta_time, PlatformInput input) {
    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)
    glm::mat4 model(1.0f);

    if (input.w_key.is_down) {
        cam.processMovementInput(molly::eMovement::kForward, delta_time);
    }
    if (input.s_key.is_down) {
        cam.processMovementInput(molly::eMovement::kBackward, delta_time);
    }
    if (input.a_key.is_down) {
        cam.processMovementInput(molly::eMovement::kLeft, delta_time);
    }
    if (input.d_key.is_down) {
        cam.processMovementInput(molly::eMovement::kRight, delta_time);
    } 

    cam.processMouseMovementInput(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    model = glm::translate(model, cube_position);
    f32 rot_angle = static_cast<f32>(glm::radians(platformGetTime())) * 30;
    model = glm::rotate(model, rot_angle, glm::vec3(1.0f));

    renderBackPack();
#ifdef GIMP
    cube.bind();
    shader.bind();
    // shader.setMat4f("model", model);
    shader.setVec3f("camera_position", cam.m_position);
    shader.setVec3f("spot_light1.position", cam.m_position);
    shader.setVec3f("spot_light1.direction", cam.m_lookat);

    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);

    for (auto& pos : container_positions) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        shader.setMat4f("model", model);
        cube.draw();
    }

    cube.unbind();
    shader.unbind();
#endif
    // Timing Information --------------------------------------------------
#ifdef MOLLY_DEBUG
    static u32 frame_count = 0;
    static f64 time_accumilator = 0.0f;
    static u32 old_fps = 0.0f;
    u32 fps = old_fps;

    time_accumilator += delta_time;
    frame_count += 1;
    if (time_accumilator >= 1.0f) {
        fps = static_cast<u32>(frame_count / time_accumilator);
        old_fps = fps;
        std::string timing = "[1s] Avg frame time: " + std::to_string(1000 * time_accumilator/frame_count) + "ms |  [1s] Avg frames per second: " + std::to_string(fps) 
        + " | Primitive Count: " + std::to_string(backpack.size());
        molly::log(timing);
        time_accumilator = 0.0f;
        frame_count = 0;
    }
#endif
}

// Services
void mollyMouseScroll(f32 yoffset) {
    cam.processMouseScrollInput(yoffset);
}