#pragma once
#include <array>
#include "Input/InputKeys.hpp"

// Forward declarations to avoid including heavy Engine headers in every file that
// uses Input. Implementation files should include the full headers when needed.
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

// ... Input API declarations would go here ...


