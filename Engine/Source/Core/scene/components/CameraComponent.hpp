//
// Created by igrom on 22.11.2025.
//

#pragma once
#include "Core/scene/Component.hpp"
#include <glm/mat4x4.hpp>

namespace LEN {
    class CameraComponent : public Component {

    public:
        void Update(float deltaTime) override;
        glm::mat4 GetViewMatrix() const;

        glm::mat4 GetProjectionMatrix() const;

    private:
    };
} // LEN
