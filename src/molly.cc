#define MOLLY_HAS_GL

// TODO
// Subsystems:
/*
Windowing + Input => OS
-----------------------
Global State
store heirearchy
Asset Pipeline
parse, cache, dispatch
Renderer
needs: Assets, inputs
updates and queries: Global state
*/

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>

#include "platform_services.h"

#include "shader.h"
#include "utils.h"
#include "camera.h"
#include "model.h"

static ModelHandle face{};
static Shader shader{};
static Camera cam{glm::vec3(1.0)};

f32 box[] = {
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

     -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
};


void mollyOnStartupCall(f32 aspect_ratio) {
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    // -- Platform Configurations --
    platformDisableMouseCursor();
    molly::printGLInfo();
    ModelData face_data =  load_model_obj(molly::resolve_path("assets/lpshead/head.OBJ"));
    MaterialHandle face_mat = load_material_to_opengl(face_data.material);
    face = load_model_to_opengl(face_data);

    shader = Shader(molly::resolve_path("src/shaders/vno_mat.glsl").c_str(), molly::resolve_path("src/shaders/fno_mat.glsl").c_str());
    
    shader.bind();
    shader.setInt("diffuse", kDiffuseTexture);
    shader.setVec3f("point_light1.position", glm::vec3(0.0, 30.0, 30.0));
    shader.setVec3f("point_light1.attenuation",glm::vec3(1.0, 0.0014, 0.000007));
    shader.setVec3f("point_light1.ambient", glm::vec3(0.1));
    shader.setVec3f("point_light1.diffuse", glm::vec3(1.0));
    shader.setVec3f("point_light1.specular", glm::vec3(1.0));
    shader.unbind();

    // cam.m_movement_speed = 50.0;
    // cam.m_far = 1500;

    /// TODO: Clean up
}

void mollyRenderLoop(f64 delta_time, PlatformInput input) {
    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)

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
    if (input.esc_key.is_down) {
        platformRequestQuit();
    }
    
    cam.processMouseMovementInput(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();
    glm::mat4 model = glm::mat4(1.0);
    
    // DRAW!!!!
    shader.bind();
    shader.setMat4f("model", model);
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);
    draw_model(face, shader);
    shader.unbind();


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
        + " | Primitive Count: " + std::to_string(/*backpack.size()*/0);
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