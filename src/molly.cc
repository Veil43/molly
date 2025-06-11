#define MOLLY_HAS_GL
#include "molly.h"

#include <glad/glad.h>
#include <string>

#include "imgui.h"

#include "platform_services.h"
#include "shader.h"
#include "utils.h"
#include "camera.h"
#include "model.h"
#include "logger.h"
#include "gltf_loader.h"
#include "input.h"

class State {
    u64 bitmap;
public:
    // State: MouseIsVisible: 1
    bool mouse_is_visible() { return bitmap & (1ull << 1); }
    void set_mouse_visibility(bool visibility) {
        if (visibility) {
            bitmap = bitmap | (1ull << 1);
        } else {
            bitmap = bitmap & ~(1ull << 1);
        }
    }

    // State: AppShouldClose: 2
    bool app_should_close() { return bitmap & (1ull << 2); }
    void set_app_should_close(bool should_close) {
        if (should_close) {
            bitmap = bitmap | (1ull << 2);
        } else {
            bitmap = bitmap & ~(1ull << 2);
        }
    }
};

static bool key_press(InputKey& k);
static bool key_hold(InputKey& k);
static bool key_release(InputKey& k);
static void imgui_debug_pannel(); /// TODO: add a debug pannel that allows us to change state at will

static Scene main_scene = {};
static Scene scene2;
static State global_state = {};

void molly_on_startup_call(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    GL_QUERY_ERROR(glDepthFunc(GL_LESS);)
    // -- Platform Configurations --
    platform_disable_mouse_cursor();

    // ------------- I/O operations ---------------------
    gltf::SceneData gltf_scene_data = gltf::load_gltf_file(utils::resolve_path("assets/models/Sponza/glTF/Sponza.gltf"));
    Shader mr_phong_shader = Shader(utils::resolve_path("src/shaders/vmr_phong.glsl"), utils::resolve_path("src/shaders/fmr_phong.glsl"));
    Shader plain_white_shader = Shader(utils::resolve_path("src/shaders/vplain_white.glsl"), utils::resolve_path("src/shaders/fplain_white.glsl"));

    register_shader_with_name("mr_phong", mr_phong_shader);
    register_shader_with_name("plain_white", plain_white_shader);

    gltf_scene_data.materials.push_back(cube::material);
    gltf::ModelData cube_model = cube::get_cube_model(gltf_scene_data.materials.size()-1);
    gltf_scene_data.models.push_back(cube_model);
    
    main_scene.handle = load_gltf_scene_to_opengl(gltf_scene_data, false);
    /// NOTE: This is hacky but brother when you gotta go you gotta go
    main_scene.handle.materials.back().shader_name = "plain_white";
    
    main_scene.camera = Camera(glm::vec3(0.0, 0.0, 0.0));
    
    main_scene.light1.type = Light::eLightType::kPoint;
    main_scene.light1.position = glm::vec3(1.0f);

    main_scene.transform.scale = glm::vec3(1.0f);
    main_scene.camera.m_movement_speed = 10.0f;
    main_scene.camera.m_position = glm::vec3(0.0f,0.0f,0.0f);
    main_scene.camera.m_far = 1000.0f;
}

void molly_render_loop(Input input) {
    static f64 delta_time = 0.0;
    delta_time = platform_measure_time_elapsed(false);

    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)

    if (key_press(input.w_key) || key_hold(input.w_key)) {
        main_scene.camera.process_movement_input(eMovement::kForward, delta_time);
    }
    if (key_press(input.s_key) || key_hold(input.s_key)) {
        main_scene.camera.process_movement_input(eMovement::kBackward, delta_time);
    }
    if (key_press(input.a_key) || key_hold(input.a_key)) {
        main_scene.camera.process_movement_input(eMovement::kLeft, delta_time);
    }
    if (key_press(input.d_key) || key_hold(input.d_key)) {
        main_scene.camera.process_movement_input(eMovement::kRight, delta_time);
    }
    if (key_press(input.esc_key)) {
        global_state.set_app_should_close(true);
    }
    if (key_hold(input.rmb)) {
        // logger::log_debug("RIGHT", logger::eLoggingLevel::kError, 30.0f);
        global_state.set_mouse_visibility(false);
        platform_disable_mouse_cursor();
    } else {
        global_state.set_mouse_visibility(true);
        platform_enable_mouse_cursor();
    }

    // Should we process the mouse input?
    if (!global_state.mouse_is_visible()) {
        main_scene.camera.process_mouse_movement_input(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);
    }

    // DRAW!!!!
    // give the point light a body
    auto& cube_model = main_scene.handle.models.back();
    cube_model.meshes[0].transform.translation = main_scene.light1.position;
    cube_model.meshes[0].transform.scale = glm::vec3(0.2/main_scene.transform.scale.x);

    f32 aspect_ratio = platform_get_screen_aspect_ratio();
    main_scene.camera.m_aspect_ratio = aspect_ratio;
    draw_molly_scene(main_scene);

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

    imgui_debug_pannel();
    logger::print_messages(delta_time);
#endif

    if (global_state.app_should_close()) {
        platform_request_quit();
    }
}

void molly_mouse_scroll(f32 yoffset) {
    main_scene.camera.process_mouse_scroll_input(yoffset);
}

// ----------------------------------------------------------------------
// Internal utils
// ----------------------------------------------------------------------
static bool key_press(InputKey& k) {
    return (k.is_down && !k.was_down);
}

static bool key_hold(InputKey& k) {
    return (k.is_down && k.was_down);
}

static bool key_release(InputKey& k) {
    return (k.was_down && !k.is_down);
}

static void imgui_debug_pannel() {
    ImGui::Begin("Debug");

    if (ImGui::CollapsingHeader("Objects")) {
        ImGui::Text("Point Light:");
        ImGui::DragFloat3("##Position", &main_scene.light1.position.x, 1.0f);
        ImGui::Text("Scene Scale");
        ImGui::DragFloat3("##Scale", &main_scene.transform.scale.x, 0.1f);
    }

    if (ImGui::CollapsingHeader("Camera Config")) {
        ImGui::Text("Far");
        ImGui::DragFloat("##Far", &main_scene.camera.m_far, 1.0f);
        ImGui::Text("Mouse Sensitivity");
        ImGui::DragFloat("##MouseSensitivity", &main_scene.camera.m_movement_speed, 1.0f);
    }

    ImGui::End();
}