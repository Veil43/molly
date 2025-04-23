#ifndef RENDERTOY_MESH_H
#define RENDERTOY_MESH_H
#include "types.h"

struct Vertex {
    struct Position { f32 x, y, z; } position;
    struct Normal { f32 x, y, z; } normal;
    struct TexCoord { f32 s, t; } tex_coord;
};

class StaticMesh {
public:
static const u32 kPositionIndex = 0;
static const u32 kNormalIndex = 1;
static const u32 kTexCoordIndex = 2;

public:
    u32 m_vbo;
    u32 m_ebo;
    u32 m_vao;
    i32 m_vertex_count;
    i32 m_index_count;

    StaticMesh(const StaticMesh& other) =delete;
    StaticMesh& operator=(const StaticMesh& other) =delete;

    StaticMesh() noexcept;    
    StaticMesh(Vertex* vertex_data, u32* index_data, i32 vcount, i32 icount);
    
    StaticMesh(StaticMesh&& other) noexcept;
    StaticMesh& operator=(StaticMesh&& other) noexcept;

    ~StaticMesh();

    void bind() const;
    void unbind() const;

    void draw() const;
};

#endif // RENDERTOY_MESH_H