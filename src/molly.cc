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

#define GL_BOUND_FBO()                                          \
    {                                                           \
        int bound;                                              \
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bound);          \
        utils::cmdlog("Bound FBO: " + std::to_string(bound));   \
    }

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

class Framebuffer {
public:
    u32 m_fbo = 0;
    u32 m_texture_color_buffer = 0;
    u32 m_depth_stencil_rbo = 0;

    Framebuffer() {}
    Framebuffer(i32 screen_width, i32 screen_height) {
        GL_QUERY_ERROR(glGenFramebuffers(1, &m_fbo);)
        GL_QUERY_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);)
        // we can have GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER also
        // for color attachment
        GL_QUERY_ERROR(glGenTextures(1, &m_texture_color_buffer);)
        GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);)
        GL_QUERY_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);)
        GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);)
        GL_QUERY_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);)
        GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, 0);)
        
        // for depth and stencil attachment
        // These are readonly
        GL_QUERY_ERROR(glGenRenderbuffers(1, &m_depth_stencil_rbo);) 
        GL_QUERY_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, m_depth_stencil_rbo);)
        GL_QUERY_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);)
        GL_QUERY_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0);)
        
        // attach the attachments
        GL_QUERY_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);)
        GL_QUERY_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_stencil_rbo);)

        GL_QUERY_ERROR(u32 status  = glCheckFramebufferStatus(GL_FRAMEBUFFER);)
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            logger::log_debug("framebuffer [" + std::to_string(m_fbo) + "] is not complete", logger::eLoggingLevel::kError, 20.0f);
        }
        unbind();
    }

    void destroy() {
        unbind();
        GL_QUERY_ERROR(glDeleteTextures(1, &m_texture_color_buffer);)
        GL_QUERY_ERROR(glDeleteRenderbuffers(1, &m_depth_stencil_rbo);)
        GL_QUERY_ERROR(glDeleteFramebuffers(1, &m_fbo);)
    }

    void bind() {
        GL_QUERY_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);)
    }
    void unbind() {
        GL_QUERY_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0);)
    }
};

void draw_quad() {

}

static bool key_press(InputKey& k);
static bool key_hold(InputKey& k);
static bool key_release(InputKey& k);
static void imgui_debug_pannel(); /// TODO: add a debug pannel that allows us to change state at will

static Scene main_scene = {};
static Scene scene2;
static State global_state = {};
static Framebuffer g_framebuffer = {};
static u32 quad_vao = 0;
static u32 quad_vbo = 0;
static Shader framebuffer_shader = {};
static bool render_frame_buffer = false;

void molly_on_startup_call(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    GL_QUERY_ERROR(glDepthFunc(GL_LESS);)
    // GL_QUERY_ERROR(glEnable(GL_STENCIL_TEST);)
    GL_QUERY_ERROR(glEnable(GL_BLEND);)
    GL_QUERY_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);)
    // -- Platform Configurations --
    platform_disable_mouse_cursor();

    // ------------- I/O operations ---------------------
    gltf::SceneData gltf_scene_data = gltf::load_gltf_file(utils::resolve_path("assets/models/Sponza/glTF/Sponza.gltf"));
    Shader mr_phong_shader = Shader(utils::resolve_path("src/shaders/vmr_phong.glsl"), utils::resolve_path("src/shaders/fmr_phong.glsl"));
    Shader plain_white_shader = Shader(utils::resolve_path("src/shaders/vplain_white.glsl"), utils::resolve_path("src/shaders/fplain_white.glsl"));

    register_shader_with_name("mr_phong", mr_phong_shader);
    register_shader_with_name("plain_white", plain_white_shader);
    
    gltf::ModelData cube_model = cube::get_cube_model(gltf_scene_data.materials.size());
    gltf_scene_data.models.push_back(cube_model);
    main_scene.handle = load_gltf_scene_to_opengl(gltf_scene_data, false);
    
    // load cube material
    gltf::MaterialInfo cube_material_data = cube::material;
    MaterialHandle cube_material = load_gltf_material_to_opengl(cube_material_data, "plain_white");
    main_scene.handle.materials.push_back(cube_material);

    main_scene.camera = Camera(glm::vec3(0.0, 0.0, 0.0));
    
    main_scene.light1.type = Light::eLightType::kPoint;
    main_scene.light1.position = glm::vec3(1.0f);

    main_scene.transform.scale = glm::vec3(1.0f);
    main_scene.camera.m_movement_speed = 10.0f;
    main_scene.camera.m_position = glm::vec3(-8.0f,1.5f,-0.4f);
    main_scene.camera.m_far = 1000.0f;

    // -------------------------------------------------
    i32 width, height;
    f32 ar;
    platform_get_screen_dimensions(width, height, ar);
    g_framebuffer = Framebuffer(width, height);

    f32 quad_vertices[] = {
         1.0f, -1.0f,    1.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,    0.0f, 1.0f,
        
         1.0f,  1.0f,     1.0f, 1.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
        -1.0f,  1.0f,    0.0f, 1.0f
    };

    GL_QUERY_ERROR(glGenVertexArrays(1, &quad_vao);)
    GL_QUERY_ERROR(glBindVertexArray(quad_vao);)
    
    GL_QUERY_ERROR(glGenBuffers(1, &quad_vbo);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);)
    GL_QUERY_ERROR(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2*sizeof(f32)));)
    GL_QUERY_ERROR(glEnableVertexAttribArray(0);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(1);)
    GL_QUERY_ERROR(glBindVertexArray(0);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0);)
    framebuffer_shader = Shader(utils::resolve_path("src/shaders/vframebuffer.glsl"),
                utils::resolve_path("src/shaders/fframebuffer.glsl"));
}

void molly_render_loop(Input input) {
    static f64 delta_time = 0.0;
    delta_time = platform_measure_time_elapsed(false);

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

    // ------------------------ Draw to offscreen buffer ------------------------
    if (render_frame_buffer) {

        g_framebuffer.bind(); // Draw to the offscreen buffer
        GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
        GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
        GL_QUERY_ERROR(glClearStencil(0);)
        GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);)
        
        // give the point light a body
        auto& cube_model = main_scene.handle.models.back();
        cube_model.meshes[0].transform.translation = main_scene.light1.position;
        cube_model.meshes[0].transform.scale = glm::vec3(0.2/main_scene.transform.scale.x);
        
        f32 aspect_ratio = platform_get_screen_aspect_ratio();
        main_scene.camera.m_aspect_ratio = aspect_ratio;
        
        draw_molly_scene(main_scene);
        g_framebuffer.unbind();
        
        // --------------------- Draw offscreen buffer to screen --------------------
        GL_QUERY_ERROR(glDisable(GL_DEPTH_TEST);)
        GL_QUERY_ERROR(glClearColor(1.0f, 1.0f, 1.0f, 1.0f);)
        GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT);)
        
        GL_QUERY_ERROR(glActiveTexture(GL_TEXTURE0 + kOtherTexUnit);)
        GL_QUERY_ERROR(glBindTexture(GL_TEXTURE_2D, g_framebuffer.m_texture_color_buffer);)
        framebuffer_shader.bind();
        GL_QUERY_ERROR(glBindVertexArray(quad_vao);)
        framebuffer_shader.set_int("screen_texture", kOtherTexUnit);
        GL_QUERY_ERROR(glDrawArrays(GL_TRIANGLES, 0, 6);)
        framebuffer_shader.unbind();

    } else {
        g_framebuffer.unbind();
        GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
        GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
        GL_QUERY_ERROR(glClearStencil(0);)
        GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);)

        // give the point light a body
        auto& cube_model = main_scene.handle.models.back();
        cube_model.meshes[0].transform.translation = main_scene.light1.position;
        cube_model.meshes[0].transform.scale = glm::vec3(0.2/main_scene.transform.scale.x);
        
        f32 aspect_ratio = platform_get_screen_aspect_ratio();
        main_scene.camera.m_aspect_ratio = aspect_ratio;
        
        draw_molly_scene(main_scene);
    }

    // -------------------------- Timing Information --------------------------
#ifdef MOLLY_DEBUG
    static u32 frame_count = 0;
    static f64 time_accumilator = 0.05;
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

void molly_on_shutdown_call() {
    g_framebuffer.destroy();
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
        ImGui::DragFloat("##MouseSensitivity", &main_scene.camera.m_mouse_sensitivity, 1.0f);
        ImGui::Text("Movement Speed");
        ImGui::DragFloat("##MovementSpeed", &main_scene.camera.m_movement_speed, 1.0f);
    }

    if (ImGui::Checkbox("Black & White Post-Process", &render_frame_buffer)) {
        // 
    }

    ImGui::End();
}