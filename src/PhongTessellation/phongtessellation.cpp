#include "phongtessellation.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"

using namespace std;

#define DIM_3 3

#define deg2rad(x) float(M_PI)*(x)/180.f

namespace  {

const string VERTEX_SHADER_PATH = "../shaders/phong_tess.vert.glsl";
const string TESSCONTROL_SHADER_PATH = "../shaders/phong_tess.tesc.glsl";
const string TESSEVAL_SHADER_PATH = "../shaders/phong_tess.tese.glsl";
const string FRAGMENT_SHADER_PATH = "../shaders/phong_tess.frag.glsl";


vector<GLfloat> vertices = {
    1,0,0,
    -1, 0, 0,
    0, 0, -1,
    0,0,1,
    0, 1, 0,
    0, -1, 0
};

vector<GLuint> indices = {
    4, 0, 2,
    4, 2, 1,
    4, 1, 3,
    4, 3, 0,
    5, 2, 0,
    5, 1, 2,
    5, 3, 1,
    5, 0, 3
};

vector<GLfloat> normals = vertices;

}

PhongTessellation::PhongTessellation(int width, int height) :
    OpenGLDemo(width, height),
    _activecamera(0),
    _camera(nullptr),
    m_mesh(GL_PATCHES),
    m_shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH,
             TESSCONTROL_SHADER_PATH, TESSEVAL_SHADER_PATH)
{

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    m_mesh.setVertices(vertices);
    m_mesh.setIndices(indices);
    m_mesh.setNormals(normals);

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f),glm::vec3(0.f, 1.f, 0.f),glm::vec3(0.f, 0.f, 0.f));} );

    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));

    _view = _camera->viewmatrix();
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void PhongTessellation::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void PhongTessellation::draw() {
    OpenGLDemo::draw();

    m_shader.use();

    _view = _camera->viewmatrix();

    m_shader.setMatrix4("model", _model);
    m_shader.setMatrix4("view", _view);
    m_shader.setMatrix4("projection", _projection);
    m_shader.setInt("tessLevel", tessLevel);

    m_mesh.draw();
}

void PhongTessellation::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void PhongTessellation::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void PhongTessellation::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
    draw();
}

bool PhongTessellation::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        case 'k':
            if(tessLevel < 30) {
                ++tessLevel;
            }
            return true;
        case 'j':
            if(tessLevel > 1) {
                --tessLevel;
            }
            return true;
        default:
            return false;
    }
}
