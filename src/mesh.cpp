#include "mesh.h"

using namespace std;

Mesh::Mesh(const int OPENGL_PRIMITIVE){
    glGenVertexArrays(1, &m_vao) ;
    glGenBuffers(1, &m_vbo) ;
    glGenBuffers(1, &m_ebo) ;
    glGenBuffers(1, &m_nbo);
    glGenBuffers(1, &m_tbo);
    m_openglPrimitive = OPENGL_PRIMITIVE;
    m_isDirty = true;
}

Mesh::Mesh(const vector<GLfloat>& vertices, const vector<GLuint>& indices, const int OPENGL_PRIMITIVE)
    : Mesh(OPENGL_PRIMITIVE)
{
    m_vertices = vertices;
    m_indices = indices;
}

Mesh::Mesh(const vector<GLfloat>& vertices, const vector<GLuint>& indices,
           const vector<GLfloat>& normals, const int OPENGL_PRIMITIVE)
    : Mesh(vertices, indices, OPENGL_PRIMITIVE)
{
    m_normals = normals;
}

Mesh::Mesh(const vector<GLfloat>& vertices, const vector<GLuint>& indices,
           const vector<GLfloat>& normals, const vector<GLfloat>& texcoords,
           const int OPENGL_PRIMITIVE)
    : Mesh(vertices, indices, normals, OPENGL_PRIMITIVE)
{
    m_texcoords = texcoords;
}

void Mesh::init() {
    glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof (GLfloat), m_vertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
            glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
            glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof (GLfloat), m_normals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*) 0);
            glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
            glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof (GLfloat), m_texcoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (GLvoid*) 0);
            glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof (GLfloat), m_indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_nbo);
    glDeleteBuffers(1, &m_tbo);
    glDeleteVertexArrays(1, &m_vao) ;
}

void Mesh::setVertices(const std::vector<GLfloat> &vertices) {
    m_vertices = vertices;
    m_isDirty = true;
}

void Mesh::setNormals(const std::vector<GLfloat> &normals) {
    m_normals = normals;
    m_isDirty = true;
}

void Mesh::setIndices(const std::vector<GLuint> &indices) {
    m_indices = indices;
    m_isDirty = true;
}

void Mesh::setTexcoords(const std::vector<GLfloat> &texCoords) {
    m_texcoords = texCoords;
    m_isDirty = true;
}

void Mesh::draw() {
    if(m_isDirty) {
        init();
    }

    glBindVertexArray(m_vao);
    glDrawElements(m_openglPrimitive, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
