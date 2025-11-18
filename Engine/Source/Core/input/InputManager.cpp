#include "InputManager.hpp"
#include <type_traits>7

namespace LEN
{
    void InputManager::SetKeyPressed(Key key, bool pressed)
    {
        auto key_index = static_cast<size_t>(key);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return;
        }
        m_keys[key_index] = pressed;
    }

	bool InputManager::IsKeyPressed(Key key) const
	{
        auto key_index = static_cast<size_t>(key);
        if (key_index == static_cast<size_t>(Key::Unknown) || key_index >= static_cast<size_t>(Key::Count)) {
            return false;
        }
		return m_keys[key_index];
	}
}

