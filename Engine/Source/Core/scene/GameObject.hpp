#pragma once
#include "Core/scene/Component.hpp"
#include <string>
#include <vector>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace LEN
{
	class GameObject
	{
	public:
		virtual ~GameObject() = default;
		virtual void Update(float deltaTime);
		const std::string& GetName() const;
		void SetName(const std::string& name); // Sets the name of the GameObject
		GameObject* GetParent(); // Get the parent GameObject, nullptr if root
		bool IsAlive() const; // Check if the GameObject is alive
		void MarkForDestroy(); // Mark the GameObject for destruction

		void AddComponent(Component* component);

		// Use glm::vec3 for positions/rotations/scales (float vectors)
		const glm::vec3& GetPosition() const;
		void SetPosition(const glm::vec3& pos);

		const glm::vec3& GetRotation() const;
		void SetRotation(const glm::vec3& rot);

		const glm::vec3& GetScale() const;
		void SetScale(const glm::vec3& scale);

		glm::mat4 GetLocalTransform() const;
		glm::mat4 GetWorldTransform() const;


	protected:
		GameObject() = default;

	private:
		std::string m_name;
		GameObject* m_parent = nullptr; // Pointer to parent GameObject, nullptr if root
		std::vector<std::unique_ptr<GameObject>> m_children; // Owned child GameObjects
		std::vector<std::unique_ptr<Component>> m_components; // Owned components
		bool m_isAlive = true; // Alive status

		// Transform properties
		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f);


		friend class Scene;
	};
}