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
            float yAngle = -deltaX * m_sensitivity * deltaTime;
            glm::quat yRot = glm::angleAxis(yAngle, glm::vec3(0.0f, 1.0f, 0.0f));

            // Rot around X axis
            float xAngle = -deltaY * m_sensitivity * deltaTime;
            glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            glm::quat xRot = glm::angleAxis(xAngle, right);

            glm::quat deltaRot = yRot * xRot;
            rotation = glm::normalize(deltaRot * rotation);

            m_owner->SetRotation(rotation);
        }

        glm::vec3 front = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
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
