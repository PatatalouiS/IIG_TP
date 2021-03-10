
#include "utils.h"
#include <cmath>
#include <gl.h>


float Utils::norm3(const GLfloat v[3]) {
    return sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

void Utils::normalize3(GLfloat v1[3]) {
    auto norme = Utils::norm3(v1);
    v1[0] /= norme;
    v1[1] /= norme;
    v1[2] /= norme;
}
