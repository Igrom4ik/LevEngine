#pragma once
#include <string>
#include <vector>
#include <memory>

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

	protected:
		GameObject() = default;

	private:
		std::string m_name;
		GameObject* m_parent = nullptr; // Pointer to parent GameObject, nullptr if root
		std::vector<std::unique_ptr<GameObject>> m_children; // Owned child GameObjects
		bool m_isAlive = true; // Alive status

		friend class Scene;
	};
}