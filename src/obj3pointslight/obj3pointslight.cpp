#include "obj3pointslight.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"
#include "shaders.h"

#define DIM_3 3

#define deg2rad(x) float(M_PI)*(x)/180.f

namespace {
    const std::string OBJ_PATH("../data/cone.obj");
}

OBJ3PointsLight::OBJ3PointsLight(int width, int height) :
    OpenGLDemo(width, height), _activecamera(0), _camera(nullptr) {

    Utils::loadOBJFile(OBJ_PATH, _vertices, _normals, _indices);

    // Initialize the geometry
    // 1. Generate geometry buffers
    glGenBuffers(1, &_vbo) ;
    glGenBuffers(1, &_nbo) ;
    glGenBuffers(1, &_ebo) ;
    glGenVertexArrays(1, &_vao) ;
    // 2. Bind Vertex Array Object
    glBindVertexArray(_vao);
        // 3. Copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size()*sizeof (GLfloat), _vertices.data(), GL_STATIC_DRAW);
        // 4. Then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // 5. Copy our normals array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, _nbo);
        glBufferData(GL_ARRAY_BUFFER, _normals.size()*sizeof (GLfloat), _normals.data(), GL_STATIC_DRAW);
        // 6. Copy our vertices array in a buffer for OpenGL to use
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        // 7. Copy our index array in a element buffer for OpenGL to use
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size()*sizeof (GLfloat), _indices.data(), GL_STATIC_DRAW);
    //6. Unbind the VAO
    glBindVertexArray(0);

    // Initialize shaders
    GLint success;
    GLchar infoLog[512]; // warning fixed size ... request for LOG_LENGTH!!!
    GLuint vertexshader, fragmentshader;


    // 1. Generate the shader
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    // 2. set the source

    glShaderSource(vertexshader, 1, &Shaders::vertex_shader_3lightning, NULL);
    glShaderSource(fragmentshader, 1, &Shaders::fragment_shader_3lightning, NULL);

    // 3. Compile
    glCompileShader(vertexshader);
    // 4. test for compile error
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 1. Generate the program
    _program = glCreateProgram();
    // 2. Attach the shaders to the program
    glAttachShader(_program, vertexshader);
    glAttachShader(_program, fragmentshader);
    // 3. Link the program
    glLinkProgram(_program);
    // 4. Test for link errors
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 7.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f),glm::vec3(0.f, 1.f, 0.f),glm::vec3(0.f, 0.f, 0.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

OBJ3PointsLight::~OBJ3PointsLight() {
    glDeleteProgram(_program);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_nbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao) ;
}

void OBJ3PointsLight::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void OBJ3PointsLight::draw() {
    OpenGLDemo::draw();

    glUseProgram(_program);

    _view = _camera->viewmatrix();

    glUniformMatrix4fv( glGetUniformLocation(_program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
    glUniformMatrix4fv( glGetUniformLocation(_program, "view"), 1, GL_FALSE, glm::value_ptr(_view));
    glUniformMatrix4fv( glGetUniformLocation(_program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void OBJ3PointsLight::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void OBJ3PointsLight::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void OBJ3PointsLight::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool OBJ3PointsLight::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        default:
            return false;
    }
}
