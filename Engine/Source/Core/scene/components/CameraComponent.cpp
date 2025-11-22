//
// Created by igrom on 22.11.2025.
//

#include "CameraComponent.hpp"
#include "Core/scene/GameObject.hpp"

namespace LEN {
    void CameraComponent::Update(float deltaTime) {

    }

    glm::mat4 CameraComponent::GetViewMatrix() const {
       return glm::inverse(m_owner->GetWorldTransform()); // Assuming m_owner is a pointer to the GameObject this component is attached to
    }

    glm::mat4 CameraComponent::GetProjectionMatrix() const {
        return  glm::mat4(1.0f);
    }
} // LEN