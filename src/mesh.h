#ifndef MESH_H
#define MESH_H

#include <gl.h>
#include <vector>
#include <QOpenGLFunctions_4_1_Core>
#include "opengl_stuff.h"

class Mesh {
    public :
        // Construct an empty Mesh
        Mesh(const int OPENGL_PRIMITIVE = GL_TRIANGLES);

        // Construct mesh with Only vertices and indices
        Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices,
             const int OPENGL_PRIMITIVE = GL_TRIANGLES);

        // Construct mesh with vertices, indices and normals
        Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices,
             const std::vector<GLfloat>& normals, const int OPENGL_PRIMITIVE = GL_TRIANGLES);

        // Construct mesh with vertices, indices, normals and texcoords
        Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices,
             const std::vector<GLfloat>& normals, const std::vector<GLfloat>& texcoords,
             const int OPENGL_PRIMITIVE = GL_TRIANGLES);

        ~Mesh();

        void setVertices(const std::vector<GLfloat>& vertices);

        inline std::vector<GLfloat>& vertices() { return m_vertices; }

        void setNormals(const std::vector<GLfloat>& normals);

        void setIndices(const std::vector<GLuint>& indices);

        void setTexcoords(const std::vector<GLfloat>& texCoords);

        void setWeights(const std::vector<GLfloat>& weights);

        inline const std::vector<GLfloat>& weights() const { return m_bonesWeights; }

        inline const std::vector<GLfloat>& initialPose() const { return m_initialPose; }

        inline GLuint getVAO() const { return m_vao; };

        inline void setDrawPrimitive(int drawPrimitive) { m_openglPrimitive = drawPrimitive; };

        void draw();

        void init();

    private :

        std::vector<GLfloat> m_vertices;
        std::vector<GLuint> m_indices;
        std::vector<GLfloat> m_normals;
        std::vector<GLfloat> m_texcoords;
        std::vector<GLfloat> m_initialPose;
        std::vector<GLfloat> m_bonesWeights;

        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ebo;
        GLuint m_nbo;
        GLuint m_tbo;
        GLuint m_wbo;

        bool m_isDirty = false;
        int m_openglPrimitive;
};

#endif // MESH_H
