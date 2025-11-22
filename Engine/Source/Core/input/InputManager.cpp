#include "InputManager.hpp"
#include <type_traits>

namespace LEN {
    void InputManager::SetKeyPressed(Key key, bool pressed) {
        auto key_index = static_cast<size_t>(key);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return;
        }
        m_keys[key_index] = pressed;
    }

    bool InputManager::IsKeyPressed(Key key) const {
        auto key_index = static_cast<size_t>(key);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return false;
        }
        return m_keys[key_index];
    }

    void InputManager::SetMouseButtonPressed(Key mouseButton, bool pressed) {
        auto key_index = static_cast<size_t>(mouseButton);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return;
        }
        m_mouseButtons[key_index] = pressed;
    }

    bool InputManager::isMouseButtonPressed(Key mouseButton) const {
        auto key_index = static_cast<size_t>(mouseButton);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return false;
        }
        return m_mouseButtons[key_index];
    }

    void InputManager::SetMousePositionOld(const glm::vec2 &pos) {
        m_mousePositionOld = pos;
    }

    const glm::vec2 &InputManager::GetMousePositionOld() const {
        return m_mousePositionOld;
    }

    void InputManager::SetMousePositionCurrent(const glm::vec2 &pos) {
        m_mousePositionCurrent = pos;
    }

    const glm::vec2 &InputManager::GetMousePositionCurrent() const {
        return m_mousePositionCurrent;
    }
}
