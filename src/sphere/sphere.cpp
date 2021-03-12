#include "sphere.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"
#include "shaders.h"

#define DIM_3 3

#define deg2rad(x) float(M_PI)*(x)/180.f

namespace  {

void addTriangleVertices(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, std::vector<GLfloat>& vertices) {
    Utils::insertBack(vertices, { v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z });
}

glm::vec3 getVertex(unsigned int index, const std::vector<GLfloat>& vertices) {
    const GLfloat* t = &vertices[index];
    return glm::vec3{t[0], t[1], t[2]};
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
    unsigned int vertex_id = 0;
    static const float div  = 22;
    static const float stepAlpha = M_PI / div;
    static const float stepBeta = (2 * M_PI) / div;

    // iterate on alpha angle, i.e height of the sphere
    for(unsigned int i = 0; i < div ; ++i) {
        float alpha = -M_PI/2 + i * stepAlpha;
        float alpha2 = -M_PI/2 + (i+1) * stepAlpha;
        unsigned int start_strip_index = vertex_id;
        // iterate on beta angle, i.e width of the sphere
        for(unsigned int j = 0; j < div - 1 ; ++j) {
            float beta = j * stepBeta;

            Utils::insertBack( _vertices, { cos(alpha) * cos(beta), sin(alpha), cos(alpha) * sin(beta) });
            Utils::insertBack( _vertices, { cos(alpha2) * cos(beta), sin(alpha2), cos(alpha2) * sin(beta) });

            Utils::insertBack( _normals, { cos(alpha) * cos(beta), sin(alpha), cos(alpha) * sin(beta) });
            Utils::insertBack( _normals, { cos(alpha2) * cos(beta), sin(alpha2), cos(alpha2) * sin(beta) });

            Utils::insertBack( _indices, { vertex_id + 1, vertex_id, vertex_id + 2});
            Utils::insertBack( _indices, { vertex_id + 2, vertex_id + 3, vertex_id + 1 });

            vertex_id += 2;
        }

        float last_strip_beta = (div - 1) * stepBeta;

        Utils::insertBack( _vertices, { cos(alpha) * cos(last_strip_beta), sin(alpha), cos(alpha) * sin(last_strip_beta) });
        Utils::insertBack( _vertices, { cos(alpha2) * cos(last_strip_beta), sin(alpha2), cos(alpha2) * sin(last_strip_beta) });

        Utils::insertBack( _normals, { cos(alpha2) * cos(last_strip_beta), sin(alpha2), cos(alpha2) * sin(last_strip_beta) });
        Utils::insertBack( _normals, { cos(alpha) * cos(last_strip_beta), sin(alpha), cos(alpha) * sin(last_strip_beta) });

        Utils::insertBack( _indices, { vertex_id + 1, vertex_id, start_strip_index });
        Utils::insertBack( _indices, { start_strip_index , start_strip_index + 1, vertex_id + 1 });

        vertex_id += 2;
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

    static const unsigned int nb_subdivision = 2;
    static constexpr unsigned int nb_vertex_icosahedron = 12;

    for(unsigned int i = 0; i < nb_subdivision; ++i) {
        unsigned int vertex_id = 0;
        std::vector<GLfloat> computed_vertices = _vertices;
        std::vector<GLuint> computed_indices = _indices;
        _vertices.clear();
        _indices.clear();

        for(unsigned int j = 0; j < computed_indices.size(); j += DIM_3) {
            // get 3 vertices of a current triangle
            glm::vec3 current_v1 = getVertex( computed_indices[j] * DIM_3, computed_vertices);
            glm::vec3 current_v2 = getVertex( computed_indices[j + 1] * DIM_3, computed_vertices);
            glm::vec3 current_v3 = getVertex( computed_indices[j + 2] * DIM_3, computed_vertices);
            // make the 3 new vertices, after have 6 vertices to make our 4 new triangles
            glm::vec3 new_v1 = normalize(current_v1 + current_v2);
            glm::vec3 new_v2 = normalize(current_v2 + current_v3);
            glm::vec3 new_v3 = normalize(current_v1 + current_v3);
            // now, we can add the vertices of 4 new triangles we obtained
            addTriangleVertices(current_v1, new_v1, new_v3, _vertices);
            addTriangleVertices(new_v1, current_v2, new_v2, _vertices);
            addTriangleVertices(new_v1, new_v2, new_v3, _vertices);
            addTriangleVertices(new_v3, new_v2, current_v3, _vertices);
            // and add the indices of 4 new triangles too
            Utils::insertBack( _indices, { vertex_id,   vertex_id + 1, vertex_id + 2 } );
            Utils::insertBack( _indices, { vertex_id + 3, vertex_id + 4, vertex_id + 5 } );
            Utils::insertBack( _indices, { vertex_id + 6, vertex_id + 7, vertex_id + 8 } );
            Utils::insertBack( _indices, { vertex_id + 9, vertex_id + 10,vertex_id + 11 } );

            vertex_id += nb_vertex_icosahedron;
        }
    }
    // we are in unitary measure, here normals are just our vertices !
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
