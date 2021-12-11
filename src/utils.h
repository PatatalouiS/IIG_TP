#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <glm/vec3.hpp>
#include <gl.h>
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


}


#endif // UTILS_H
