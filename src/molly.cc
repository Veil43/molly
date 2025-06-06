#define MOLLY_HAS_GL
#include "molly.h"

// TODO
// Subsystems:
/*
// Create a state object that is accessible everywhere.
*/

#include <glad/glad.h>
#include <string>

#include "platform_services.h"
#include "shader.h"
#include "utils.h"
#include "camera.h"
#include "model.h"
#include "logger.h"
#include "gltf_loader.h"

static bool key_press(PlatformKey& k);
static bool key_hold(PlatformKey& k);
static bool key_release(PlatformKey& k);
static void imgui_debug_pannel();

static Shader shader{};
static Camera cam{glm::vec3(1.0)};
static bool sg_mouse_is_visible = true;
static gSceneHandle scene = {};

void molly_on_startup_call(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    // -- Platform Configurations --
    sg_mouse_is_visible = false;
    platform_disable_mouse_cursor();

    gltf::Scene backpack = gltf::load_gltf_file(utils::resolve_path("assets/Sponza/glTF/Sponza.gltf"));
    scene = load_gltf_scene_to_opengl(backpack, false);
    
    shader = Shader(utils::resolve_path("src/shaders/vno_mat.glsl").c_str(), utils::resolve_path("src/shaders/fmr_phong.glsl").c_str());
    
    shader.bind();
    shader.set_vec3f("point_light1.position", glm::vec3(0.0, 30.0, 30.0));
    shader.set_vec3f("point_light1.attenuation",glm::vec3(1.0, 0.0014, 0.000007));
    shader.set_vec3f("point_light1.ambient", glm::vec3(0.1));
    shader.set_vec3f("point_light1.diffuse", glm::vec3(1.0));
    shader.set_vec3f("point_light1.specular", glm::vec3(1.0));
    shader.unbind();
    cam.m_movement_speed = 1.0f;
    cam.m_position = glm::vec3(0.0f,0.0f,0.0f);
    cam.m_far = 10000.0f;
}

void molly_render_loop(PlatformInput input) {
    static f64 delta_time = 0.0;
    delta_time = platform_measure_time_elapsed(false);

    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)

    if (key_press(input.w_key) || key_hold(input.w_key)) {
        cam.process_movement_input(utils::eMovement::kForward, delta_time);
    }
    if (key_press(input.s_key) || key_hold(input.s_key)) {
        cam.process_movement_input(utils::eMovement::kBackward, delta_time);
    }
    if (key_press(input.a_key) || key_hold(input.a_key)) {
        cam.process_movement_input(utils::eMovement::kLeft, delta_time);
    }
    if (key_press(input.d_key) || key_hold(input.d_key)) {
        cam.process_movement_input(utils::eMovement::kRight, delta_time);
    } 
    if (key_press(input.esc_key)) {
        platform_request_quit();
    }
    if (key_press(input.lctrl_key)) {
        sg_mouse_is_visible = !sg_mouse_is_visible;
        if (sg_mouse_is_visible) {
            platform_enable_mouse_cursor();
        } else {
            platform_disable_mouse_cursor();
        }
    }

    // Should we process the mouse input?
    if (!sg_mouse_is_visible) {
        cam.process_mouse_movement_input(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);
    }

    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = cam.get_projection_matrix();
    glm::mat4 model = glm::mat4(1.0);
    
    // DRAW!!!!
    shader.bind();
    // shader.set_mat4f("model", model); // The model matrix is held in individual mesh
    shader.set_mat4f("view", view);
    shader.set_mat4f("projection", projection);
    draw_gltf_scene(scene, shader);
    shader.unbind();
       
    // -------------------------- Timing Information --------------------------
#ifdef MOLLY_DEBUG
    static u32 frame_count = 0;
    static f64 time_accumilator = 0.0;
    static u32 old_fps = 0;
    u32 fps = old_fps;

    time_accumilator += delta_time;
    frame_count += 1;
    if (time_accumilator >= 1.0) {
        fps = static_cast<u32>(frame_count / time_accumilator);
        old_fps = fps;
        std::string timing = "[1s] Avg frame time: " + std::to_string(1000 * time_accumilator/frame_count) + "ms |  [1s] Avg frames per second: " + std::to_string(fps) 
        + " | Primitive Count: " + std::to_string(/*backpack.size()*/0);
        // logger::log_debug(timing, logger::eLoggingLevel::kInfo, 3.0);
        time_accumilator = 0.0;
        frame_count = 0;
    }

    std::string message = "FPS: " + std::to_string(fps);
    logger::write_text_at(message, 10.0, 10.0);

    logger::print_messages(delta_time);
#endif
}

void molly_mouse_scroll(f32 yoffset) {
    cam.process_mouse_scroll_input(yoffset);
}

// ----------------------------------------------------------------------
// Internal utils
// ----------------------------------------------------------------------
static bool key_press(PlatformKey& k) {
    return (k.is_down && !k.was_down);
}

static bool key_hold(PlatformKey& k) {
    return (k.is_down && k.was_down);
}

static bool key_release(PlatformKey& k) {
    return (k.was_down && !k.is_down);
}