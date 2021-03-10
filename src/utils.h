#ifndef UTILS_H
#define UTILS_H



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

float norm3(const float v[3]);

void normalize3(float v1[3]);

}


#endif // UTILS_H
