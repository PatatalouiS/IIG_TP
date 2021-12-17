#ifndef ANIMATION_H
#define ANIMATION_H

#include "opengldemo.h"

#include "../camera/camera.h"

#include <memory>
#include <functional>
#include "../utils.h"
#include "../mesh.h"
#include "../shader.h"
#include "bone.h"
#include <chrono>

/** Simple drawing demonstration
 */
class Animation : public OpenGLDemo {
public:
    explicit Animation(int width, int height);
    ~Animation() override;

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

private:

    // for mouse management
    int _button; // 0 --> left. 1 --> right. 2 --> middle. 3 --> other
    float _mousex{0};
    float _mousey{0};

    // Camera
    using CameraSelector=std::function<Camera*()>;
    void initBones();
    void initWeights();
    void updateMeshVertices();
    std::vector<CameraSelector> _cameraselector;
    unsigned int _activecamera;

    std::unique_ptr<Camera> _camera;

    Mesh m_mesh;
    Shader m_shader;

    // Bones architecture
    std::unique_ptr<Bone> m_rootBone;

    // time management
    std::chrono::steady_clock::time_point start;
    double elapsedTime();

    // matrices
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;

    bool m_useGPU = true;
    bool m_activateRotation = true;
    bool m_activateTranslation = false;
    bool m_activateScale = false;
};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // ANIMATION_H
