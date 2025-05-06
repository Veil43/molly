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

static StaticMesh cube{};
static std::vector<StaticMesh> backpack{};

static glm::vec3 cube_position(0.0f, 0.0f, -4.0f);
static Shader shader{};
static Texture container_diffuse_map{};
static Texture container_specular_map{};
static Camera cam{};
static u32* texture_handles;
static std::vector<molly::MaterialData> s_materials;

void configureTexture(const molly::TextureConfig& config) {
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.min_filter);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.mag_filter);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.wrap_s);)
    GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.wrap_t);)
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

    // survival_guitar_backpack
    molly::ModelData backpack_model = molly::loadModel("../data/teapot/scene.gltf");
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
    // shader = Shader("../src/shaders/vsample.glsl", "../src/shaders/fsample.glsl");
    shader = Shader("../src/shaders/vphong.glsl", "../src/shaders/fphong.glsl");
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
    PointLight point_light1 = {};
    point_light1.position = glm::vec3(200.0f);
    point_light1.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    point_light1.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    point_light1.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    point_light1.specular = glm::vec3(1.0f);

    shader.bind();
    shader.setVec3f("camera_position", cam.m_position);
    shader.setVec3f("point_light1.position", point_light1.position);
    shader.setVec3f("point_light1.attenuation", point_light1.position);
    shader.setVec3f("point_light1.ambient", point_light1.ambient);
    shader.setVec3f("point_light1.diffuse", point_light1.diffuse);
    shader.setVec3f("point_light1.specular", point_light1.specular);
    shader.unbind();
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

void render3DObject(f64 dt) {
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();
    f32 rotate_speed = 1.0e1f;

    shader.bind();
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);    
    shader.setVec3f("camera_position", cam.m_position);

    f64 angle = static_cast<f64>(glm::radians(GLFWGetTime()));
    angle *= rotate_speed;
    for (auto& mesh : backpack) {
        glm::mat4 model = mesh.m_transform;
        model = glm::rotate(model, (f32)(angle), glm::vec3(0.0f, 1.0f, 0.0f));
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

    render3DObject(delta_time);

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