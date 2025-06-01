#define MOLLY_HAS_GL
#include <glad/glad.h>
#include <iostream>

#include "model.h"
#include "utils.h"

#include <fstream>
#include <sstream>
#include <cstdio>
#include <cassert>

/// NOTE: in the blender project repository see geometry/GEO_mesh_triangulate.hh

ModelHandle load_model_to_opengl(ModelData& m) {
    u32 vao;
    GL_QUERY_ERROR(glGenVertexArrays(1, &vao);)
    GL_QUERY_ERROR(glBindVertexArray(vao);)
    u32 vbo;
    u32 ebo;
    GL_QUERY_ERROR(glGenBuffers(1, &vbo);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vbo);)
    GL_QUERY_ERROR(glBufferData(GL_ARRAY_BUFFER, m.vcount * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glVertexAttribPointer(kPositionIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));)
    GL_QUERY_ERROR(glVertexAttribPointer(kNormalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));)
    GL_QUERY_ERROR(glVertexAttribPointer(kTexCoordIndex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kPositionIndex);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kNormalIndex);)
    GL_QUERY_ERROR(glEnableVertexAttribArray(kTexCoordIndex);)

    GL_QUERY_ERROR(glGenBuffers(1, &ebo);)
    GL_QUERY_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);)
    GL_QUERY_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.icount * sizeof(i32), m.indices.data(), GL_STATIC_DRAW);)
    GL_QUERY_ERROR(glBindVertexArray(0);)
    GL_QUERY_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0);)
    GL_QUERY_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);)

    ModelHandle output_handle = {};
    output_handle.vao = vao;
    output_handle.vbo = vbo;
    output_handle.ebo = ebo;
    output_handle.vcount = m.vcount;
    output_handle.icount = m.icount;
    return output_handle;
}

/// NOTE: loses the texture ID we will not be needing that
MaterialHandle load_material_to_opengl(MaterialData& m) {
    MaterialHandle output_material = {};
    
    output_material.diff_image = molly::loadImageFile(m.diffuse.c_str());
    output_material.norm_image = molly::loadImageFile(m.normal.c_str());
    output_material.spec_image = molly::loadImageFile(m.specular.c_str());

    output_material.diff_texture = molly::load_image_to_opengl(output_material.diff_image, kDiffuseTexture);
    output_material.norm_texture = molly::load_image_to_opengl(output_material.norm_image, kNormalTexture);
    output_material.spec_texture = molly::load_image_to_opengl(output_material.spec_image, kSpecularTexture);

    return output_material;
}

static glm::vec3 parse_vec3(const std::string& line) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;
    float x, y, z;
    iss >> x >> y >> z;
    // printf("%s: %f %f %f\n", prefix.c_str(), x, y, z);
    return glm::vec3(x,y,z);
}

static glm::vec2 parse_vec2(const std::string& line) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;
    float x, y;
    iss >> x >> y;
    // printf("vec2: %f %f\n", x, y);
    return glm::vec2(x,y);
}

/*
    Assumes triangles
*/
static auto generate_normals(const std::vector<Vertex>& vertices, 
                             const std::vector<i32>& indices) -> std::vector<glm::vec3>
{
    /*
                    p2        

        p0              p1
        The winding order is counter clockwise
    */
    std::vector<glm::vec3> output_normals(vertices.size(), glm::vec3(0.0));
    int size = indices.size() / 3;
    for (int i = 0; i < size; i++) {
        int i0 = indices[i*3];
        int i1 = indices[i*3+1];
        int i2 = indices[i*3+2];

        glm::vec3 p0 = vertices[i0].position;
        glm::vec3 p1 = vertices[i1].position;
        glm::vec3 p2 = vertices[i2].position;

        glm::vec3 edge01 = p1-p0;
        glm::vec3 edge02 = p2-p0;
        glm::vec3 edge12 = p2-p1;

        // oh boy!
        glm::vec3 n0 = glm::normalize(glm::cross(edge01,edge02));
        glm::vec3 n1 = glm::normalize(glm::cross(edge12, -edge01));
        glm::vec3 n2 = glm::normalize(glm::cross(-edge02, -edge12));

        output_normals[i0] += n0;
        output_normals[i1] += n1;
        output_normals[i2] += n2;
    }

    for (auto& n : output_normals) {
        n = glm::normalize(n);
    }

    return (output_normals); // moves (i think)
}

struct TriFaces {
    std::vector<i32> indices;
};

static std::vector<i32> triangulate_face(const std::vector<i32>& indices) {
    int size = indices.size();
    if (size != 4) { /// TODO: remove me
        DEBUG_BREAK
    }
    std::vector<i32> output_indices;
    output_indices.reserve(6);

    if (indices.size() == 4) {
        output_indices.push_back(indices[0]);
        output_indices.push_back(indices[1]);
        output_indices.push_back(indices[2]);

        output_indices.push_back(indices[0]);
        output_indices.push_back(indices[2]);
        output_indices.push_back(indices[3]);
        return output_indices;
    }
    /// TODO: add ear clipping

    return output_indices;
}

struct IndexTriple {
    i32 p, t, n;
};

struct Face {
    std::vector<IndexTriple> indices;
};

static Face parse_face(const std::string& line) {
    std::istringstream iss(line);
    std::string index;
    std::string prefix;
    iss >> prefix;
    Face output_face;
    
    while(iss >> index) {
        i32 indices[3] = {-1,-1,-1};
        i32 fslash_count = 0;
        std::string val = "";
    
        for (int i = 0; i <= index.size(); i++) {
            const char c = index[i];
            assert(fslash_count<3);
            if (c!='/' && c!= '\0') {
                val += c;
                continue;
            }

            if (val == "") {
                indices[fslash_count] = -1;
            } else {
                indices[fslash_count] = std::atoi(val.c_str());
            }
            fslash_count++;
            val = "";
        }
        IndexTriple i{};
        i.p = indices[0];
        i.t = indices[1];
        i.n = indices[2];

        output_face.indices.push_back(i);
    }

    return output_face;
}

static MaterialData parse_material(const std::string& dir, const std::string& file) {
    std::string path = dir + file;
    std::string true_path = molly::resolve_path(path);
    std::ifstream ifs(path);
    MaterialData output_material = {};
    if (!ifs.is_open()) {
        std::cerr << "ERROR::I/O: could not load file with path : <" << path << ">\n";
        return output_material;
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::string line;
    std::istringstream file_contents (oss.str());

    /// NOTE: only take the first to be mentioned (for now)

    bool diff_found = false;
    bool norm_found = false;
    bool spec_found = false;

    while(std::getline(file_contents, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        /// TODO: support Ka, Kd and Ks
        prefix = molly::tolower(prefix);
        if (prefix == "map_kd" && !diff_found) {
            std::string file;
            iss >> file;
            output_material.diffuse = dir + file; 
            diff_found = true;
        } else if ((prefix == "map_bump" || prefix == "bump") && !norm_found) {

            std::cout << "Line for bump: " << line << "\n";
            std::string file;
            iss >> file;
            output_material.normal = file;
            norm_found = true;
        } else if (prefix == "map_ks" && !spec_found) {
            std::string file;
            iss >> file;
            output_material.specular = file;
            spec_found = true;
        }

        if (diff_found && norm_found && spec_found) {
            break;
        }
    }

    return output_material;
}

ModelData load_model_obj(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR::I/O: Could not open file <" << path << ">\n";
        return ModelData{};
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::istringstream stream(oss.str());
    std::string line;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
    // std::unordered_map<std::string, std::vector<i32>> groups;

    std::vector<Face> faces;
    std::string material_file;
    while (std::getline(stream, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        // Material data
        if (prefix == "mtllib") {
            iss >> material_file;
            continue;
        }

        // Vertex data
        if (prefix == "v") {
            auto p = parse_vec3(line);
            positions.push_back(p);
        } else if (prefix == "vn") {
            auto n = parse_vec3(line);
            normals.push_back(n);
        } else if (prefix=="vt") {
             auto t = parse_vec2(line);
             tex_coords.push_back(t);
        } else if (prefix=="f") {
            auto f = parse_face(line);
            faces.push_back(f);
        }
    }

    // create vertices (will need to duplicate some of the data)
    std::vector<Vertex> vertices;
    std::vector<i32> indices;

    int curr_index = 0;
    for (auto& face: faces) {
        std::vector<i32> temp_indices;
        temp_indices.reserve(4);
        for (auto& index_triple : face.indices) {
            Vertex v = {};
            if (index_triple.p >= 0) {
                v.position = positions[index_triple.p-1];
            }
            if (index_triple.t >= 0 && index_triple.t <= tex_coords.size()) {
                v.tex_coord = tex_coords[index_triple.t-1];
            }
            if (index_triple.n >= 0 && index_triple.n <= normals.size()) {
                v.normal = positions[index_triple.n-1];
            }
            temp_indices.push_back(curr_index);
            vertices.push_back(v);
            curr_index++;
        }

        // triangulate
        if (temp_indices.size()>3) {
            auto triangles = triangulate_face(temp_indices);
            indices.insert(indices.end(), triangles.begin(), triangles.end());
        } else {
            indices.insert(indices.end(), temp_indices.begin(), temp_indices.end());
        }
    }

    if (normals.empty()) {
        normals = generate_normals(vertices, indices);
        assert(normals.size() == vertices.size());
        for (int i =0; i<vertices.size(); i++) {
            vertices[i].normal = normals[i];
        }
    }

    u32 fslash_pos = path.find_last_of("/");
    std::string material_directory = path.substr(0,fslash_pos + 1);
    std::cout << material_directory << " END!\n";
    MaterialData material = parse_material(material_directory, material_file);

    ModelData output_model;
    output_model.vcount = vertices.size();
    output_model.icount = indices.size();
    output_model.vertices = std::move(vertices);
    output_model.indices = std::move(indices);
    output_model.material = std::move(material);

    return output_model; /// TODO: Does it move?
}

void draw_model(ModelHandle& m, Shader& shader) {
    shader.bind();
    GL_QUERY_ERROR(glBindVertexArray(m.vao);)
    if (m.icount > 0) {
        GL_QUERY_ERROR(glDrawElements(GL_TRIANGLES, m.icount, GL_UNSIGNED_INT, 0);)
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m.vcount);
    }
    GL_QUERY_ERROR(glBindVertexArray(0);)
    shader.unbind();
}
