#define RENDERTOY_HAS_GL
#include <glad/glad.h>
#include <cstddef>
#include "mesh.h"
#include "rdtmath.h"
#include "utils.h"

StaticMesh::StaticMesh() noexcept
    : m_vertex_count{0}, m_index_count{0}, m_vao{0}, m_vbo{0}, m_ebo{0}
{}

StaticMesh::StaticMesh(Vertex* vertex_data, u32* index_data, i32 vcount, i32 icount, const glm::mat4& transform)
    : m_vertex_count{vcount}, m_index_count{icount}, m_transform{transform}
{
    this->populateOpenGLBuffers(vertex_data, index_data);
}

StaticMesh::StaticMesh(const rdt::MeshData& data) 
    : m_vertex_count{data.vertex_count}, m_index_count{data.index_count}, m_transform{data.world_transform}, m_material_index{data.material_index}
{
    Vertex* vertex_data = (Vertex*)data.vertex_data;
    u32* index_data = data.index_data;
    this->populateOpenGLBuffers(vertex_data, index_data);
}

StaticMesh::StaticMesh(StaticMesh&& other) noexcept {
    if (this != &other) {
        this->m_vbo = other.m_vbo;
        this->m_ebo = other.m_ebo;
        this->m_vao = other.m_vao;
        this->m_vertex_count = other.m_vertex_count;
        this->m_index_count = other.m_index_count;
        this->m_transform = other.m_transform;
        
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vao = 0;
        other.m_vertex_count = 0;
        other.m_index_count = 0;
        other.m_transform = glm::mat4(0.0f);
    }
}

StaticMesh& StaticMesh::operator=(StaticMesh&& other) noexcept {
    if (this != &other) {
        this->m_vbo = other.m_vbo;
        this->m_ebo = other.m_ebo;
        this->m_vao = other.m_vao;
        this->m_vertex_count = other.m_vertex_count;
        this->m_index_count = other.m_index_count;
        this->m_transform = other.m_transform;

        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vao = 0;
        other.m_vertex_count = 0;
        other.m_index_count = 0;
        other.m_transform = glm::mat4(0.0f);
    }
    return *this;
}

StaticMesh::~StaticMesh() {
    if (m_vbo) {
        GL_QUERY_ERROR(glDeleteBuffers(1, &m_vbo);)
        m_vbo = 0;
    }
    if (m_ebo) {
        GL_QUERY_ERROR(glDeleteBuffers(1, &m_ebo);)
        m_ebo = 0;
    }
    if (m_vao) {
        GL_QUERY_ERROR(glDeleteVertexArrays(1, &m_vao);)
        m_vao = 0;
    }
}

// Public Member functions -------------------
void StaticMesh::bind() const {
    GL_QUERY_ERROR(glBindVertexArray(m_vao);)
}
void StaticMesh::unbind() const {
    GL_QUERY_ERROR(glBindVertexArray(0);)
}
void StaticMesh::draw() const {
    this->bind();
    if (this->m_index_count > 0) {
        GL_QUERY_ERROR(glDrawElements(GL_TRIANGLES, this->m_index_count, GL_UNSIGNED_INT, 0);)
    } else {
        GL_QUERY_ERROR(glDrawArrays(GL_TRIANGLES, 0, this->m_vertex_count);)
    }
    this->unbind();
}

// Private member functions --------------

void StaticMesh::populateOpenGLBuffers(Vertex* vertex_data, u32* index_data) {
    GL_QUERY_ERROR(glGenVertexArrays(1, &this->m_vao);)
    GL_QUERY_ERROR(glBindVertexArray(this->m_vao);)
    GL_QUERY_ERROR(glGenBuffers(1, &this->m_vbo);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, this->m_vertex_count * sizeof(Vertex), (void*)vertex_data, GL_STATIC_DRAW);)
    if (index_data != nullptr) {
        GL_QUERY_ERROR(glGenBuffers(1, &this->m_ebo);)
        GL_QUERY_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);)
        GL_QUERY_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_index_count * sizeof(u32), (void*)index_data, GL_STATIC_DRAW);)
    } else {
        this->m_index_count = 0;
    }
    GL_QUERY_ERROR(glVertexAttribPointer(kPositionIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));)
    GL_QUERY_ERROR(glVertexAttribPointer(kNormalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));)
    GL_QUERY_ERROR(glVertexAttribPointer(kTexCoordIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kPositionIndex);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kNormalIndex);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kTexCoordIndex);)
    // More attributes go here -----------
    GL_QUERY_ERROR(glBindVertexArray(0);)
}