//
// Created by igrom on 03.12.2025.
//

#include "LightComponent.hpp"

namespace LEN {
    void LightComponent::Update(float deltaTime) {
    }

    void LightComponent::setColor(glm::vec3 color) {
        m_color = color;
    }

    const glm::vec3 &LightComponent::GetColor() const {
        return m_color;
    }
} // LEN
