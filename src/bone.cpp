#include "bone.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "utils.h"

using Utils::degToRad;
using Utils::IDENTITY;
using namespace std;

Bone::Bone(unsigned int id) :
    m_boneID(id),
    m_rotation(0,0,0),
    m_translation(0,0,0),
    m_scale(1,1,1),
    m_transform(IDENTITY)
{};

void Bone::addChildBone(unsigned int child_id) {
    m_children.push_back(make_unique<Bone>(child_id));
}

glm::mat4 Bone::getTransformMatrix() const {
   auto rotationX = glm::rotate(IDENTITY, glm::radians(m_rotation.x), { 1.0, 0, 0 });
   auto rotationY = glm::rotate(IDENTITY, glm::radians(m_rotation.y), { 0, 1.0, 0 });
   auto rotationZ = glm::rotate(IDENTITY, glm::radians(m_rotation.z), { 0, 0, 1.0 });
   auto translation = glm::translate(IDENTITY, m_translation);
   auto scale = glm::scale(IDENTITY, m_scale);

   return translation * (rotationZ * rotationY * rotationX) * scale;
}

void Bone::resetAllTransforms() {
    m_rotation = { 0, 0, 0 };
    m_translation = { 0, 0, 0 };
    m_scale = { 1, 1, 1};
    m_transform = IDENTITY;

    // Apply recursively to all children
    for(auto& child : m_children) {
        child->resetAllTransforms();
    }
}

void Bone::resetAllRotations() {
    m_rotation = { 0, 0, 0 };

    // Apply recursively to all children
    for(auto& child : m_children) {
        child->resetAllRotations();
    }
}

void Bone::resetAllScales() {
    m_scale = { 1,1,1 };

    // Apply recursively to all children
    for(auto& child : m_children) {
        child->resetAllScales();
    }
}

void Bone::resetAllTranslations() {
    m_translation = { 0,0,0 };

    // Apply recursively to all children
    for(auto& child : m_children) {
        child->resetAllTranslations();
    }
}

