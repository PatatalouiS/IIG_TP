#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <glm.hpp>
#include <QString>
#include "mesh.h"


namespace Utils {

enum class Demo {
    NORMALS_VIEW,
    ERROR_VIEW,
    LIGHT_VIEW
};

enum class Type {
    PARAMETRIC,
    ICOSAHEDRON
};

template<typename T>
void insertBack(std::vector<T>& vec, const std::initializer_list<T>&& list) {
    vec.insert(vec.end(), std::move(list));
}

void loadOBJFile(const std::string& path, std::vector<GLfloat>& vertices,
                                      std::vector<GLfloat>& normals,
                                      std::vector<GLuint>& indices );

std::string read_shader_file (const std::string& shaderFile);

void loadOBJFile2(const std::string &fileName, std::vector<GLfloat>& vertices,
                     std::vector<GLuint>& indices, std::vector<GLfloat>& normals,
                  std::vector<GLfloat>& coord);

Mesh makeCylinder(glm::vec3 base=glm::vec3(0,0,0),
                  glm::vec3 axis = glm::vec3(0,1,0),
                  float radius = .5f,
                  float length = 3.,
                  int subdiv1 = 32,
                  int subdiv2 = 32);


inline GLfloat degToRad(GLfloat degree) {
    return degree / (M_PI * 180.f);
}

static const glm::mat4 IDENTITY(1.0f);

}




#endif // UTILS_H
