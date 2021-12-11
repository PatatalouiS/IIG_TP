#ifndef WINDOW_DEMO_H
#define WINDOW_DEMO_H

#include "opengldemo.h"

#include "../camera/camera.h"

#include <memory>
#include <functional>
#include "../utils.h"
#include "../mesh.h"
#include "../shader.h"

/** Simple drawing demonstration
 */
class PNTriangles : public OpenGLDemo {
public:
    explicit PNTriangles(int width, int height);
    ~PNTriangles() override;

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
    unsigned int tessLevel = 5;

    // Camera
    using CameraSelector=std::function<Camera*()>;
    std::vector<CameraSelector> _cameraselector;
    unsigned int _activecamera;

    std::unique_ptr<Camera> _camera;

    Mesh m_mesh;
    Shader m_shader;

    // matrices
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // WINDOW_DEMO
