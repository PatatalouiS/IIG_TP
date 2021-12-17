#include "animation.h"
#include <iostream>
#include <QDebug>
#include <array>
#include "../utils.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cmath>

using namespace std;
using namespace chrono;

#define DIM_3 3
#define ROOT_BONE 0
#define CHILD_BONE 1
#define MAX_ANGLE_ANIM 90
#define MAX_TRANSLATION_ANIM 2
#define MAX_SCALE 1.3
#define MIN_SCALE 0.75

namespace  {

const string VERTEX_SHADER_PATH = "../shaders/animation.vert.glsl";
const string FRAGMENT_SHADER_PATH = "../shaders/animation.frag.glsl";

}


Animation::Animation(int width, int height) :
    OpenGLDemo(width, height),
    _activecamera(0), _camera(nullptr),
    m_mesh(Utils::makeCylinder()),
    m_shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH)
{
    start = std::chrono::steady_clock::now();
    m_mesh.setDrawPrimitive(GL_TRIANGLES);
    initBones();
    initWeights();

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 4.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void Animation::initBones() {
    m_rootBone = make_unique<Bone>(ROOT_BONE);
    m_rootBone->addChildBone(CHILD_BONE);
}

void Animation::initWeights() {
    auto& vert = m_mesh.vertices();
    vector<GLfloat> weights;

    for(unsigned int i = 0; i < vert.size(); i += DIM_3) {
        glm::vec3 vertice(vert[i], vert[i+1], vert[i+2]);
        auto weight_root = (vertice.y + 1.5f) / 3.f;
        auto weight_child = 1 - weight_root;
        weights.push_back(weight_root);
        weights.push_back(weight_child);
    }

    m_mesh.setWeights(std::move(weights));
}

void Animation::updateMeshVertices() {
    auto& initialPose = m_mesh.initialPose();
    auto& bonesWeights = m_mesh.weights();
    auto vertices = m_mesh.vertices();
    auto rootBoneTransform = m_rootBone->getTransformMatrix();
    auto childBoneTransform = m_rootBone->firstChild()->getTransformMatrix();

    for(unsigned int i = 0; i < initialPose.size(); i += DIM_3) {
        glm::vec4 initialVertex({ initialPose[i], initialPose[i+1], initialPose[i+2], 1.0 });
        auto vertexWeights = &bonesWeights[(i  / DIM_3) * 2];

        auto weightedTransform = rootBoneTransform * vertexWeights[0] +
                                 childBoneTransform * vertexWeights[1];

        auto newVertex = weightedTransform * initialVertex;
        vertices[i] = newVertex.x; vertices[i + 1] = newVertex.y; vertices[i + 2] = newVertex.z;
    }

    m_mesh.setVertices(vertices);
}


Animation::~Animation() {
}

void Animation::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void Animation::draw() {
    OpenGLDemo::draw();

    // ANIMATION
    auto t = elapsedTime();
    // ROTATION
    if(m_activateRotation) {
        m_rootBone->firstChild()->setRotationZ(cos(t * 2) * MAX_ANGLE_ANIM);
    }
    // TRANSLATION
    if(m_activateTranslation) {
        m_rootBone->setTranslationX(cos(t) * MAX_TRANSLATION_ANIM);
        m_rootBone->firstChild()->setTranslationX(cos(t) * MAX_TRANSLATION_ANIM);
    }
    // SCALE
    if(m_activateScale) {
        m_rootBone->firstChild()->setScale(glm::vec3(1,1,1) * float(cos(t) * 0.5f + 1));
    }
    // if we use CPU, we need to update our mesh
    if(!m_useGPU) {
        updateMeshVertices();
    }

    m_shader.use();
    _view = _camera->viewmatrix();

    m_shader.setMatrix4("model", _model);
    m_shader.setMatrix4("view", _view);
    m_shader.setMatrix4("projection", _projection);

    // in case we use GPU, pass transformation matrices
    m_shader.setMatrix4("rootTransform", m_rootBone->getTransformMatrix());
    m_shader.setMatrix4("childTransform", m_rootBone->firstChild()->getTransformMatrix());
    m_shader.setBool("useGPU", m_useGPU);

    m_mesh.draw();
}

void Animation::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void Animation::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void Animation::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

double Animation::elapsedTime() {
    auto time = duration_cast<milliseconds>(steady_clock::now() - start);
    // time in seconds, with 3 decimals !
    return time.count() / 1000.f;
}

bool Animation::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        case 'g' :
            m_useGPU = !m_useGPU;

            if(m_useGPU) {
                m_mesh.setVertices(m_mesh.initialPose());
                cout << "USING GPU (Graphics)" << endl;
            }
            else {
                cout << "USING CPU (Processor)" << endl;
            }
            return true;
        case 'r' :
            m_activateRotation = !m_activateRotation;
            if(!m_activateRotation) {
                m_rootBone->resetAllRotations();
                cout << "ROTATION : OFF" << endl;
            }
            else {
                cout << "ROTATION : ON" << endl;
            }
            return true;
        case 't' :
            m_activateTranslation = !m_activateTranslation;
            if(!m_activateTranslation) {
                m_rootBone->resetAllTranslations();
                cout << "TRANSLATION : OFF" << endl;
            }
            else {
                cout << "TRANSLATION : ON" << endl;
            }
            return true;
        case 's' :
            m_activateScale = !m_activateScale;
            if(!m_activateScale) {
                m_rootBone->resetAllScales();
                cout << "SCALE : OFF" << endl;
            }
            else {
                cout << "SCALE : ON" << endl;
            }
            return true;
        default:
            return false;
    }
}
