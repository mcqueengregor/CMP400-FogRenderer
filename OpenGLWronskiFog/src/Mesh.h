#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Buffers.h"
#include "GLErrorManager.h"

#include <string>
#include <vector>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<VAO::Vertex>    m_vertices;
    std::vector<GLuint>         m_indices;
    std::vector<Texture>        m_textures;
    VAO m_VAO;
    GLuint m_vao;

    Mesh(const std::vector<VAO::Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
    void draw(const Shader& shader) const;

private:
    EBO m_EBO;
    GLuint m_ebo;
    GLuint m_vbo;
    void setupMesh();
};
#endif