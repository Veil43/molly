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

#include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#include "molly.h"

static ModelHandle face{};
static Shader shader{};
static Camera cam{glm::vec3(1.0)};

void molly_on_startup_call(f32 aspect_ratio) {
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    // -- Platform Configurations --
    platform_disable_mouse_cursor();
    molly::print_GL_info();
    ModelData face_data =  load_model_obj(molly::resolve_path("assets/backpack/backpack.OBJ"));
    MaterialHandle face_mat = load_material_to_opengl(face_data.material, false);
    face = load_model_to_opengl(face_data);

    shader = Shader(molly::resolve_path("src/shaders/vno_mat.glsl").c_str(), molly::resolve_path("src/shaders/fno_mat.glsl").c_str());
    
    shader.bind();
    shader.set_int("map_diffuse", kDiffuseTexture);
    shader.set_int("map_normal", kNormalTexture);
    shader.set_int("map_specular", kSpecularTexture);
    shader.set_vec3f("point_light1.position", glm::vec3(0.0, 30.0, 30.0));
    shader.set_vec3f("point_light1.attenuation",glm::vec3(1.0, 0.0014, 0.000007));
    shader.set_vec3f("point_light1.ambient", glm::vec3(0.1));
    shader.set_vec3f("point_light1.diffuse", glm::vec3(1.0));
    shader.set_vec3f("point_light1.specular", glm::vec3(1.0));
    shader.unbind();
}

void molly_render_loop(PlatformInput input) {
    static f64 delta_time = 0.0;
    delta_time = platform_measure_time_elapsed(false);

    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)

    if (input.w_key.is_down) {
        cam.process_movement_input(molly::eMovement::kForward, delta_time);
    }
    if (input.s_key.is_down) {
        cam.process_movement_input(molly::eMovement::kBackward, delta_time);
    }
    if (input.a_key.is_down) {
        cam.process_movement_input(molly::eMovement::kLeft, delta_time);
    }
    if (input.d_key.is_down) {
        cam.process_movement_input(molly::eMovement::kRight, delta_time);
    } 
    if (input.esc_key.is_down) {
        platform_request_quit();
    }
    
    cam.process_mouse_movement_input(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);

    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = cam.get_projection_matrix();
    glm::mat4 model = glm::mat4(1.0);
    
    // DRAW!!!!
    shader.bind();
    shader.set_mat4f("model", model);
    shader.set_mat4f("view", view);
    shader.set_mat4f("projection", projection);
    draw_model(face, shader);
    shader.unbind();
    
    // LOG
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddText(ImVec2(10, 10), IM_COL32(255, 255, 0, 255), "Hello from the top-left!");


    // Timing Information --------------------------------------------------
#ifdef MOLLY_DEBUG
    ImGui::Begin("Debug Window");

    ImGui::Text("Delta Time: %.3f ms", delta_time * 1000.0);
    ImGui::Text("FPS: %d", (int)(1.0 / delta_time));

    // You can add checkboxes, sliders, etc. too
    // ImGui::Checkbox("Wireframe Mode", &some_debug_bool);

    ImGui::End();

    // static u32 frame_count = 0;
    // static f64 time_accumilator = 0.0;
    // static u32 old_fps = 0;
    // u32 fps = old_fps;

    // time_accumilator += delta_time;
    // frame_count += 1;
    // if (time_accumilator >= 1.0) {
    //     fps = static_cast<u32>(frame_count / time_accumilator);
    //     old_fps = fps;
    //     std::string timing = "[1s] Avg frame time: " + std::to_string(1000 * time_accumilator/frame_count) + "ms |  [1s] Avg frames per second: " + std::to_string(fps) 
    //     + " | Primitive Count: " + std::to_string(/*backpack.size()*/0);
    //     molly::log(timing);
    //     time_accumilator = 0.0;
    //     frame_count = 0;
    // }
#endif
}

// Services
void molly_mouse_scroll(f32 yoffset) {
    cam.process_mouse_scroll_input(yoffset);
}