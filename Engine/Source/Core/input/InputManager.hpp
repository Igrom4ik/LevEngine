#pragma once
#include <array>
#include "Core/Input/InputKeys.hpp"

namespace LEN
{
    class InputManager
    {
    private:
		InputManager() = default;
		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator = (const InputManager&) = delete;
		InputManager& operator = (InputManager&&) = delete;
		~InputManager() = default;
    public:

		void SetKeyPressed(Key key, bool pressed);
		bool IsKeyPressed(Key key) const;

	private:
		std::array<bool, static_cast<size_t>(Key::Count)> m_keys{};
		friend class Engine;
    };
}



