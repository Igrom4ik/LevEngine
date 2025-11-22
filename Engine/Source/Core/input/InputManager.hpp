#pragma once
#include <array>
#include <glm/vec2.hpp>
#include "Core/Input/InputKeys.hpp"

namespace LEN {
	class InputManager {
	private:
		InputManager() = default;

		InputManager(const InputManager &) = delete;

		InputManager(InputManager &&) = delete;

		InputManager &operator =(const InputManager &) = delete;

		InputManager &operator =(InputManager &&) = delete;

		~InputManager() = default;

	public:
		void SetKeyPressed(Key key, bool pressed);

		bool IsKeyPressed(Key key) const;

		void SetMouseButtonPressed(Key mouseButton, bool pressed);

		bool isMouseButtonPressed(Key mouseButton) const;

		void SetMousePositionOld(const glm::vec2 &pos);

		const glm::vec2 &GetMousePositionOld() const;

		void SetMousePositionCurrent(const glm::vec2 &pos);

		const glm::vec2 &GetMousePositionCurrent() const;

	private:
		std::array<bool, static_cast<size_t>(Key::Count)> m_keys{};
		std::array<bool, static_cast<size_t>(Key::Count)> m_mouseButtons{};
		glm::vec2 m_mousePositionOld = glm::vec2(0.0f);
		glm::vec2 m_mousePositionCurrent = glm::vec2(0.0f);
		friend class Engine;
	};
}



