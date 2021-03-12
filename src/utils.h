#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <glm/vec3.hpp>
#include <gl.h>
#include <QString>

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



}


#endif // UTILS_H
