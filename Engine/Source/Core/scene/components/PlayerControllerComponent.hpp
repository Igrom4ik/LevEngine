//
// Created by igrom on 22.11.2025.
//

#pragma once
#include "Core/scene/Component.hpp"

namespace LEN {
    class PlayerControllerComponent : public Component {
        COMPONENT(PlayerControllerComponent) // Macro to define component type


        void Update(float deltaTime) override;

    private:
        float m_sensitivity = 0.5f;
        float m_moveSpeed = 1.0f;
    };
}
