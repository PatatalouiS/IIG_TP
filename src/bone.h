#ifndef BONE_H
#define BONE_H

#include <glm.hpp>
#include <memory>
#include <vector>
#include <opengl_stuff.h>

class Bone {

public :

    Bone(unsigned int id);

    inline unsigned int ID() const { return m_boneID; }

    void addChildBone(unsigned int child_id);

    std::unique_ptr<Bone>& firstChild() { return m_children.front(); }

    glm::mat4 getTransformMatrix() const;

    // Rotation getters and setters
    inline const glm::vec3& rotation() const { return m_rotation; }

    inline void setRotation(glm::vec3 newRotation) { m_rotation = newRotation; }

    inline GLfloat rotationX() const { return m_rotation.x; };

    inline void setRotationX(GLfloat newRotationX) { m_rotation.x = newRotationX; }

    inline GLfloat rotationY() const { return m_rotation.y; };

    inline void setRotationY(GLfloat newRotationY) { m_rotation.y = newRotationY; }

    inline GLfloat rotationZ() const { return m_rotation.z; };

    inline void setRotationZ(GLfloat newRotationZ) { m_rotation.z = newRotationZ; }

    // Translation getters and setters
    inline const glm::vec3& traslation() const { return m_translation; }

    inline void setTranslation(glm::vec3 newTranslation) { m_translation = newTranslation; }

    inline GLfloat TranslationX() const { return m_translation.x; };

    inline void setTranslationX(GLfloat newTranslationX) { m_translation.x = newTranslationX; }

    inline GLfloat TranslationY() const { return m_translation.y; };

    inline void setTranslationY(GLfloat newTranslationY) { m_translation.y = newTranslationY; }

    inline GLfloat TranslationZ() const { return m_translation.z; };

    inline void setTranslationZ(GLfloat newTranslationZ) { m_translation.z = newTranslationZ; }

    // Scale getters and setters
    inline const glm::vec3& scale() const { return m_scale; }

    inline void setScale(glm::vec3 newScale) { m_scale = newScale; }

    inline GLfloat ScaleX() const { return m_scale.x; };

    inline void setScaleX(GLfloat newScaleX) { m_scale.x = newScaleX; }

    inline GLfloat ScaleY() const { return m_scale.y; };

    inline void setScaleY(GLfloat newScaleY) { m_scale.y = newScaleY; }

    inline GLfloat ScaleZ() const { return m_scale.z; };

    inline void setScale(GLfloat newScaleZ) { m_scale.z = newScaleZ; }

    void resetAllTransforms();

    void resetAllRotations();

    void resetAllTranslations();

    void resetAllScales();

private :

    unsigned int m_boneID = 0;

    glm::vec3 m_rotation;
    glm::vec3 m_translation;
    glm::vec3 m_scale;

    glm::mat4 m_transform;

    std::vector<std::unique_ptr<Bone>> m_children;
};


#endif // BONE_H
