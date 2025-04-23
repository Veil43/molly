#define RENDERTOY_HAS_GL

#include "rendertoy.h"
#include <glad/glad.h>
#include <stb_image.h>

#include <string>
#include <iostream>
#include <iomanip>

#include "shader.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "math.h"

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
static Texture texture{};
void renderToyOnStartupCall(f32 aspect_ratio) {
    // --- OpenGL Configurations ---
    GL_QUERY_ERROR(glEnable(GL_DEPTH_TEST);)

    rdt::printGLInfo();
    cube = StaticMesh((Vertex*)square_vertices, nullptr, 36, 0);
    shader = Shader("../src/shaders/vsource.vsh", "../src/shaders/fsource.fsh");
    texture = Texture("../data/container.png");

    u32 texture_unit = 0;
    shader.bind();
    shader.setInt("container", texture_unit);
    texture.bind(texture_unit);
    shader.unbind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, cube_position);
    // model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f));

    glm::vec3 cam_position(0.0f, 0.0f, 4.0f);
    glm::vec3 world_up(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cam_position, cube_position, world_up);

    glm::mat4 projection = glm::perspective(glm::radians(20.0f), aspect_ratio, 0.1f, 100.0f);

    shader.bind();
    shader.setMat4f("model", model);
    shader.setMat4f("view", view);
    shader.setMat4f("projection", projection);
    shader.bind();
}

void renderToyRenderLoop(f32 delta_time) {
    GL_QUERY_ERROR(glClearColor(0.5, 0.7, 0.9, 1.0f);)
    GL_QUERY_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);)
    glm::mat4 model(1.0f);
    
    cube.bind();
    shader.bind();
    
    model = glm::translate(model, cube_position);
    f32 rot_angle = static_cast<f32>(glm::radians(GLFWGetTime())) * 30;
    model = glm::rotate(model, rot_angle, glm::vec3(1.0f));
    shader.setMat4f("model", model);

    GL_QUERY_ERROR(glDrawArrays(GL_TRIANGLES, 0, 36);)
    cube.unbind();
    shader.unbind();
}