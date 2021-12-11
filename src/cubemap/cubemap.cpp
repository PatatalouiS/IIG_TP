#include "cubemap.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"
#include "../stb_image.h"

using namespace std;

#define DIM_3 3

#define deg2rad(x) float(M_PI)*(x)/180.f

namespace  {

const string VERTEX_SHADER_PATH = "../shaders/cubemap.vert.glsl";
const string FRAGMENT_SHADER_PATH = "../shaders/cubemap.frag.glsl";
const string VERTEX_SHADER_PATH2 = "../shaders/cubemap2.vert.glsl";
const string FRAGMENT_SHADER_PATH2 = "../shaders/cubemap2.frag.glsl";

vector<GLfloat> vertices = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
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

CubeMap::CubeMap(int width, int height) :
    OpenGLDemo(width, height),
    _activecamera(0),
    _camera(nullptr),
    m_meshCubeMap(GL_PATCHES),
    m_shaderCubeMap(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH),
    m_shaderOBJ(VERTEX_SHADER_PATH2, FRAGMENT_SHADER_PATH2)
{

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    m_meshCubeMap.setVertices(vertices);
    m_meshCubeMap.init();

    vector<string> faces {
      "../data/left.jpg",
      "../data/right.jpg",
      "../data/up.jpg",
      "../data/down.jpg",
      "../data/back.jpg",
      "../data/front.jpg"
    };

    glGenTextures(1, &m_textureCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureCubeMap);

    int w, h, nbc;

    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &w, &h, &nbc, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    vector<GLfloat> _normals;
    vector<GLuint> _indices;
    vector<GLfloat> _texcoord;
    vector<GLfloat> _vertices;

    Utils::loadOBJFile2("../data/cube.obj", _vertices, _indices, _normals, _texcoord);

    m_obj.setVertices(_vertices);
    m_obj.setIndices(_indices);
    m_obj.setNormals(_normals);
    m_obj.setTexcoords(_texcoord);

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(-0.5, 0, 0));} );

    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));

    _view = _camera->viewmatrix();
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void CubeMap::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void CubeMap::draw() {
    OpenGLDemo::draw();
    _view = _camera->viewmatrix();

    m_shaderOBJ.use();
    m_shaderOBJ.setMatrix4("model", glm::translate(glm::scale(_model, {0.5, 0.5, 0.5}), {1, 0.5, -5}));
    m_shaderOBJ.setMatrix4("view", _view);
    m_shaderOBJ.setMatrix4("projection", _projection);
    m_shaderOBJ.setVec3("camera", _camera->position());
    m_shaderOBJ.setBool("mode", m_mode);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureCubeMap);
    m_obj.draw();

    glDepthFunc(GL_LEQUAL);
    m_shaderCubeMap.use();

    m_shaderCubeMap.setMatrix4("model", glm::scale(_model, {20, 20, 20}));
    m_shaderCubeMap.setMatrix4("view", glm::mat4(glm::mat3(_view)));
    m_shaderCubeMap.setMatrix4("projection", _projection);

    glBindVertexArray(m_meshCubeMap.getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureCubeMap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

void CubeMap::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void CubeMap::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void CubeMap::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool CubeMap::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        case 'm':
            m_mode = !m_mode;
            return true;
        default:
            return false;
    }
}
