#pragma once
#include "Core/scene/GameObject.hpp"
#include <string>
#include <vector>
#include <memory>
#include <type_traits>


namespace LEN
{
	class GameObject;

	class Scene
	{
	public:
		void Update(float deltaTime);
		void Clear();

		GameObject* CreateObject(const std::string& name, GameObject* parent = nullptr);

		template<typename T, typename = typename std::enable_if_t<std::is_base_of_v<GameObject, T>>>
		T* CreateObject(const std::string& name, GameObject* parent = nullptr)
		{
			auto obj = std::make_unique<T>();
			obj->SetName(name);
			GameObject* raw = obj.get();
			if (parent)
			{
				parent->m_children.push_back(std::move(obj));
				raw->m_parent = parent;
			}
			else
			{
				m_objects.push_back(std::move(obj));
				raw->m_parent = nullptr;
			}
			return static_cast<T*>(raw);
		}

		bool SetParent(GameObject* obj, GameObject* parent);

		void SetMainCamera(GameObject* camera);
		GameObject* GetMainCamera();

	private:
		std::vector<std::unique_ptr<GameObject>> m_objects;
		GameObject* m_mainCamera = nullptr;
	};
}