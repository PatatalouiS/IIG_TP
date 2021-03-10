#include "parametric_sphere.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"
#include "shaders.h"

#define DIM_3 3

#define deg2rad(x) float(M_PI)*(x)/180.f

namespace  {

void add_vec(const GLfloat v1[3], const GLfloat v2[3], GLfloat newV[3])
{
    newV[0] = v1[0] + v2[0];    // x
    newV[1] = v1[1] + v2[1];    // y
    newV[2] = v1[2] + v2[2];    // z
    Utils::normalize3(newV);
}

void addTriangle(const GLfloat v1[3], const GLfloat v2[3], const GLfloat v3[3], std::vector<GLfloat>& vec, std::vector<GLfloat>& vec2) {
    vec.insert(vec.end(), {v1[0], v1[1], v1[2]});
    vec.insert(vec.end(), {v2[0], v2[1], v2[2]});
    vec.insert(vec.end(), {v3[0], v3[1], v3[2]});
}

void addIndices(unsigned int a, unsigned int b, unsigned int c, std::vector<GLuint>& vec) {
    vec.insert(vec.end(), { a, b, c });
}

}

Sphere::Sphere(int width, int height, Utils::Demo typeDemo, Utils::Type typeSphere) :
    OpenGLDemo(width, height), _activecamera(0), _camera(nullptr) {

    if(typeSphere == Utils::Type::PARAMETRIC)
        makeParametricSphere();
    else
        makeIcosahedralSphere();

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

    switch(typeDemo) {
        case Utils::Demo::NORMALS_VIEW :
            glShaderSource(vertexshader, 1, &Shaders::vertex_shader_normals, NULL);
            glShaderSource(fragmentshader, 1, &Shaders::fragment_shader_normals, NULL);
            break;
        case Utils::Demo::ERROR_VIEW :
            glShaderSource(vertexshader, 1, &Shaders::vertex_shader_error, NULL);
            glShaderSource(fragmentshader, 1, &Shaders::fragment_shader_error, NULL);
            break;
        case Utils::Demo::LIGHT_VIEW :
            glShaderSource(vertexshader, 1, &Shaders::vertex_shader_lightning, NULL);
            glShaderSource(fragmentshader, 1, &Shaders::fragment_shader_lightning, NULL);
            break;
        default:
            break;
    }

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

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f),glm::vec3(0.f, 1.f, 0.f),glm::vec3(0.f, 0.f, 0.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void Sphere::makeParametricSphere() {
    float alpha;
    float alpha2;
    float beta;
    unsigned int vertex_id = 0;
    static const float div  = 20;
    static const float stepAlpha = M_PI / div;
    static const float stepBeta = (2 * M_PI) / div;

    for(unsigned int i = 0; i < div ; ++i) {
        alpha = -M_PI/2 + i * stepAlpha;
        alpha2 = -M_PI/2 + (i+1) * stepAlpha;
        unsigned int start_strip_index = vertex_id;

        for(unsigned int j = 0; j < div - 1 ; ++j) {
            beta = j * stepBeta;

            _vertices.insert(_vertices.end(), { cos(alpha) * cos(beta), sin(alpha), cos(alpha) * sin(beta) });
            _normals.insert(_normals.end(), { cos(alpha) * cos(beta), sin(alpha), cos(alpha) * sin(beta) });
            _vertices.insert(_vertices.end(), { cos(alpha2) * cos(beta), sin(alpha2), cos(alpha2) * sin(beta) });
            _normals.insert(_normals.end(), { cos(alpha2) * cos(beta), sin(alpha2), cos(alpha2) * sin(beta) });

            _indices.insert(_indices.end(), { vertex_id + 1, vertex_id, vertex_id + 2});
            _indices.insert(_indices.end(), { vertex_id + 2, vertex_id + 3, vertex_id + 1 });

            vertex_id += 2;

        }

        float last_strip_beta = (div - 1) * stepBeta;
        _vertices.insert(_vertices.end(), { cos(alpha) * cos(last_strip_beta), sin(alpha), cos(alpha) * sin(last_strip_beta) });
        _normals.insert(_normals.end(), { cos(alpha) * cos(last_strip_beta), sin(alpha), cos(alpha) * sin(last_strip_beta) });
        _vertices.insert(_vertices.end(), { cos(alpha2) * cos(last_strip_beta), sin(alpha2), cos(alpha2) * sin(last_strip_beta) });
        _normals.insert(_normals.end(), { cos(alpha2) * cos(last_strip_beta), sin(alpha2), cos(alpha2) * sin(last_strip_beta) });

        vertex_id += 2;

        _indices.insert(_indices.end(), { vertex_id - 1, vertex_id - 2 , start_strip_index });
        _indices.insert(_indices.end(), { start_strip_index , start_strip_index + 1, vertex_id -1 });
    }
}

void Sphere::makeIcosahedralSphere() {
    // the gold number
    //static const double goldNumber = (1 + sqrt(5)) / 2;
    // factor to multipy the edges of the gold rectangle, to have a perfectly normalized sphere
    //static const double normalizationFactor = 2.f / sqrt(1 + goldNumber * goldNumber);
    // now we can define our two values for the icosahedron mesh
    //static const double X_coor = normalizationFactor / 2;
    //static const double Z_coor = (goldNumber * normalizationFactor) / 2;

    #define X 0.525731112119133606
    #define Z 0.850650808352039932

    _vertices = {
        -X, 0.0, Z,
        X, 0.0, Z,
        -X, 0.0, -Z,
        X, 0.0, -Z,
        0.0, Z, X,
        0.0, Z, -X,
        0.0, -Z, X,
        0.0, -Z, -X,
        Z, X, 0.0,
        -Z, X, 0.0,
        Z, -X, 0.0,
        -Z, -X, 0.0
    };

    _indices = {
        0,4,1,
        0,9,4,
        9,5,4,
        4,5,8,
        4,8,1,
        8,10,1,
        8,3,10,
        5,3,8,
        5,2,3,
        2,7,3,
        7,10,3,
        7,6,10,
        7,11,6,
        11,0,6,
        0,1,6,
        6,1,10,
        9,0,11,
        9,11,2,
        9,2,5,
        7,2,11
    };

    const auto nb_subdiv = 2;
    std::vector<GLfloat> computed_vertices;
    std::vector<GLuint> computed_indices;
    const GLfloat *v1, *v2, *v3;
    GLfloat newV1[3], newV2[3], newV3[3];

    for(unsigned int i = 0; i < nb_subdiv; ++i) {
        auto index = 0;
        computed_vertices = _vertices;
        computed_indices = _indices;
        _vertices.clear();
        _indices.clear();

        for(unsigned int j = 0; j < computed_indices.size(); j += DIM_3) {
            // get 3 vertices of a triangle
            v1 = &computed_vertices[computed_indices[j] * 3];
            v2 = &computed_vertices[computed_indices[j + 1] * 3];
            v3 = &computed_vertices[computed_indices[j + 2] * 3];

            add_vec(v1, v2, newV1);
            add_vec(v2, v3, newV2);
            add_vec(v1, v3, newV3);

            addTriangle(v1, newV1, newV3, _vertices, _normals);
            addTriangle(newV1, v2, newV2, _vertices, _normals);
            addTriangle(newV1, newV2, newV3, _vertices, _normals);
            addTriangle(newV3, newV2, v3, _vertices, _normals);

            addIndices(index,   index+1, index+2, _indices);
            addIndices(index+3, index+4, index+5, _indices);
            addIndices(index+6, index+7, index+8, _indices);
            addIndices(index+9, index+10,index+11, _indices);
            index += 12;
        }
    }

    _normals = _vertices;
}


Sphere::~Sphere() {
    glDeleteProgram(_program);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_nbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao) ;
}

void Sphere::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void Sphere::draw() {
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

void Sphere::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void Sphere::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void Sphere::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool Sphere::keyboard(unsigned char k) {
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
