#ifndef RENDERTOY_MESH_H
#define RENDERTOY_MESH_H
#include "types.h"
#include "utils.h"
#include "rdtmath.h"

class StaticMesh {
public:
static const u32 kPositionIndex = 0;
static const u32 kNormalIndex = 1;
static const u32 kTexCoordIndex = 2;

public:
    glm::mat4 m_transform = glm::mat4(1.0f);
    u32 m_vbo;
    u32 m_ebo;
    u32 m_vao;
    i32 m_vertex_count;
    i32 m_index_count;

    // temp
    u32 m_material_index = 0;
    StaticMesh(const StaticMesh& other) =delete;
    StaticMesh& operator=(const StaticMesh& other) =delete;

    StaticMesh() noexcept;    
    StaticMesh(Vertex* vertex_data, u32* index_data, i32 vcount, i32 icount, const glm::mat4& transform = glm::mat4(1.0f));
    StaticMesh(const rdt::MeshData& data);

    StaticMesh(StaticMesh&& other) noexcept;
    StaticMesh& operator=(StaticMesh&& other) noexcept;

    ~StaticMesh();

    void bind() const;
    void unbind() const;

    void draw() const;
private:
    void populateOpenGLBuffers(Vertex* vertex_data, u32* index_data);
};

#endif // RENDERTOY_MESH_H