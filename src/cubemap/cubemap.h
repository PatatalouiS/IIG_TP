#ifndef CUBEMAP_H
#define CUBEMAP_H


#include "opengldemo.h"

#include "../camera/camera.h"

#include <memory>
#include <functional>
#include "../utils.h"

#include "../mesh.h"
#include "../shader.h"

/** Simple drawing demonstration
 */
class CubeMap : public OpenGLDemo {
public:
    explicit CubeMap(int width, int height);

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

private:

    void makeParametricSphere();
    void makeIcosahedralSphere();

    // for mouse management
    int _button; // 0 --> left. 1 --> right. 2 --> middle. 3 --> other
    float _mousex{0};
    float _mousey{0};
    bool m_mode = true;

    // Camera
    using CameraSelector=std::function<Camera*()>;
    std::vector<CameraSelector> _cameraselector;
    unsigned int _activecamera;

    std::unique_ptr<Camera> _camera;

    // matrices
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;

    // Meshs to draw
    Mesh m_meshCubeMap;
    Mesh m_obj;

    // Shader
    Shader m_shaderCubeMap;
    Shader m_shaderOBJ;

    // Textures
    GLuint m_textureCubeMap;
};

#endif // CUBEMAP_H
