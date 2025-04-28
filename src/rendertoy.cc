#define RENDERTOY_HAS_GL

#include "rendertoy.h"
#include <glad/glad.h>
#include <stb_image.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#include "platform_services.h"

#include "shader.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "rdtmath.h"
#include "camera.h"
#include "material.h"
#include "light.h"

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
    u8* data = stbi_load(texture_path.c_str(), &width, &height, &channel_count, 0);

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
static glm::vec3 cube_position(0.0f, 0.0f, -4.0f);
static Shader shader{};
static Texture container_diffuse_map{};
static Texture container_specular_map{};
static Camera cam{};

void renderToyOnStartupCall(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)
    // -- Platform Configurations --
    platformDisableMouseCursor();

    rdt::printGLInfo();
    cube = StaticMesh((Vertex*)square_vertices, nullptr, 36, 0);
    shader = Shader("../src/shaders/vsample.glsl", "../src/shaders/fsample.glsl");
    container_diffuse_map = Texture("../data/container.png");
    container_specular_map= Texture("../data/container_specular.png");

    shader.bind();
    shader.setInt("container.diffuse_map", RDT_DIFFUSE_MAP_TEXTURE_UNIT);
    shader.setInt("container.specular_map", RDT_SPECULAR_MAP_TEXTURE_UNIT);
    shader.setFloat("container.shininess", 32.0f);
    container_diffuse_map.bind(RDT_DIFFUSE_MAP_TEXTURE_UNIT);
    container_specular_map.bind(RDT_SPECULAR_MAP_TEXTURE_UNIT);
    shader.unbind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, cube_position);
    // model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f));
    
    Camera tmp(glm::vec3(0.0f, 0.0f, 3.0f));
    cam = tmp;

    cam.m_aspect_ratio = aspect_ratio;
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    PointLight point_light1 = {};
    point_light1.position = glm::vec3(1.0f);
    point_light1.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    point_light1.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    point_light1.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    point_light1.specular = glm::vec3(1.0f);

    SpotLight spot_light1 = {};
    spot_light1.position = cam.m_position;

    spot_light1.direction = -spot_light1.position;
    spot_light1.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);
    spot_light1.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    spot_light1.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    spot_light1.specular = glm::vec3(1.0f);
    spot_light1.cutoff = glm::cos(glm::radians(12.5f));
    spot_light1.outer_cutoff = glm::cos(glm::radians(17.5f));

    shader.bind();
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
}

std::vector<glm::vec3> container_positions = 
{
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

void renderToyRenderLoop(f64 delta_time, PlatformInput input) {
    GL_QUERY_ERROR(glClearColor(0.01f, 0.01f, 0.01f, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)
    glm::mat4 model(1.0f);

    cube.bind();
    shader.bind();
    if (input.w_key.is_down) {
        cam.processMovementInput(rdt::eMovement::kForward, delta_time);
    }
    if (input.s_key.is_down) {
        cam.processMovementInput(rdt::eMovement::kBackward, delta_time);
    }
    if (input.a_key.is_down) {
        cam.processMovementInput(rdt::eMovement::kLeft, delta_time);
    }
    if (input.d_key.is_down) {
        cam.processMovementInput(rdt::eMovement::kRight, delta_time);
    } 

    cam.processMouseMovementInput(input.mouse_x - input.mouse_prevx, input.mouse_prevy - input.mouse_y, delta_time);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    model = glm::translate(model, cube_position);
    f32 rot_angle = static_cast<f32>(glm::radians(platformGetTime())) * 30;
    model = glm::rotate(model, rot_angle, glm::vec3(1.0f));

    // shader.setMat4f("model", model);
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

    // Timing Information --------------------------------------------------
#ifdef RENDERTOY_DEBUG
    static u32 frame_count = 0;
    static f64 time_accumilator = 0.0f;
    static u32 old_fps = 0.0f;
    u32 fps = old_fps;

    time_accumilator += delta_time;
    frame_count += 1;
    if (time_accumilator >= 1.0f) {
        fps = static_cast<u32>(frame_count / time_accumilator);
        old_fps = fps;
        std::string timing = "[1s] Avg frame time: " + std::to_string(1000 * time_accumilator/frame_count) + "ms |  [1s] Avg frames per second: " + std::to_string(fps);
        rdt::log(timing);
        time_accumilator = 0.0f;
        frame_count = 0;
    }
#endif
}

// Services
void renderToyMouseScroll(f32 yoffset) {
    cam.processMouseScrollInput(yoffset);
}