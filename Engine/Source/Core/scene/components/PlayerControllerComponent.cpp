//
// Created by igrom on 22.11.2025.
//

#include "PlayerControllerComponent.hpp"
#include "Core/input/InputManager.hpp"
#include "Core/Engine.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace LEN {
    void PlayerControllerComponent::Update(float deltaTime) {
        auto &inputManager = Engine::GetInstance().GetInputManager();
        auto rotation = m_owner->GetRotation();

        if (inputManager.isMouseButtonPressed(LEN::Key::MouseLeft)) {
            const auto &oldPos = inputManager.GetMousePositionOld();
            const auto &currentPos = inputManager.GetMousePositionCurrent();

            float deltaX = currentPos.x - oldPos.x;
            float deltaY = currentPos.y - oldPos.y;

            // Rot around Y axis
            rotation.y -= deltaX * m_sensitivity * deltaTime;

            // Rot around X axis
            rotation.x -= deltaY * m_sensitivity * deltaTime;
            m_owner->SetRotation(rotation);
        }

        glm::mat4 rotMat(1.0f);
        rotMat = glm::rotate(rotMat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotMat = glm::rotate(rotMat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotMat = glm::rotate(rotMat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 front = glm::normalize(glm::vec3(rotMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
        glm::vec3 right = glm::normalize(glm::vec3(rotMat * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
        auto position = m_owner->GetPosition();

        auto &input = LEN::Engine::GetInstance().GetInputManager();

        // Horizontal movement
        if (input.IsKeyPressed(LEN::Key::A) || input.IsKeyPressed(LEN::Key::Left)) {
            position -= right * m_moveSpeed * deltaTime;
        } else if (input.IsKeyPressed(LEN::Key::D) || input.IsKeyPressed(LEN::Key::Right)) {
            position += right * m_moveSpeed * deltaTime;
        }

        // Vertical movement
        if (input.IsKeyPressed(LEN::Key::S) || input.IsKeyPressed(LEN::Key::Down)) {
            position -= front * m_moveSpeed * deltaTime;
        } else if (input.IsKeyPressed(LEN::Key::W) || input.IsKeyPressed(LEN::Key::Up)) {
            position += front * m_moveSpeed * deltaTime;
        }
        m_owner->SetPosition(position);
    }
}
