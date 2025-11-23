//
// Created by igrom on 22.11.2025.
//

#include "CameraComponent.hpp"
#include "Core/scene/GameObject.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace LEN {
    void CameraComponent::Update(float deltaTime) {
    }


    glm::mat4 CameraComponent::GetViewMatrix() const {
        // Build world transform: rotation then translation (column-major, mat[3] is translation)
        auto mat = glm::mat4_cast(m_owner->GetRotation());

        // GLM uses column-major matrices; mat[3] is the translation column (vec4).
        // Setting mat[3] = glm::vec4(position, 1.0f) constructs the world transform
        // where the translation is stored in the 4th column. The view matrix is
        // the inverse of the world transform, so we return glm::inverse(mat).
        mat[3] = glm::vec4(m_owner->GetPosition(), 1.0f);


        if (m_owner->GetParent()) {
            mat = m_owner->GetParent()->GetWorldTransform() * mat;
        }

        return glm::inverse(mat);
    }

    glm::mat4 CameraComponent::GetProjectionMatrix(float aspectRat) const {
        return glm::perspective(glm::radians(m_fov), aspectRat, m_nearPlane, m_farPlane);
    }
} // LEN
