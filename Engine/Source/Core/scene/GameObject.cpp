#include "Core/scene/GameObject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace LEN
{
	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_components) {
			component->Update(deltaTime);
		}

		for (auto it = m_children.begin(); it != m_children.end();)
		{
			if ((*it)->IsAlive())
			{
				(*it)->Update(deltaTime);
				++it;
			}
			else
			{
				it = m_children.erase(it);
			}
		}

	}

	const std::string& GameObject::GetName() const
	{
		return m_name;
	}

	void GameObject::SetName(const std::string& name)
	{
		m_name = name;
	}

	GameObject* GameObject::GetParent()
	{
		return m_parent;
	}

	bool GameObject::IsAlive() const
	{
		return m_isAlive;
	}

	void GameObject::MarkForDestroy()
	{
		m_isAlive = false;
	}

	void GameObject::AddComponent(Component *component) {
		m_components.emplace_back(component);
		component->m_owner = this;
	}

	// Transform accessors
	const glm::vec3& GameObject::GetPosition() const
	{
		return m_position;
	}

	void GameObject::SetPosition(const glm::vec3& pos)
	{
		m_position = pos;
	}

	const glm::vec3& GameObject::GetRotation() const
	{
		return m_rotation;
	}

	void GameObject::SetRotation(const glm::vec3& rot)
	{
		m_rotation = rot;
	}

	const glm::vec3& GameObject::GetScale() const
	{
		return m_scale;
	}

	void GameObject::SetScale(const glm::vec3& scale)
	{
		m_scale = scale;
	}

	// Compute local transformation matrix
	glm::mat4 GameObject::GetLocalTransform() const
	{
		glm::mat4 mat = glm::mat4(1.0f); // Identity matrix

		// Translation
		mat = glm::translate(mat, m_position);

		// Rotation (assuming m_rotation is in radians)
		mat = glm::rotate(mat, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X
		mat = glm::rotate(mat, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y
		mat = glm::rotate(mat, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z

		// Scale
		mat = glm::scale(mat, m_scale);

		return mat;
	}

	// Compute world transformation matrix
	glm::mat4 GameObject::GetWorldTransform() const
	{
		if (m_parent)
		{
			return m_parent->GetWorldTransform() * GetLocalTransform();
		}
		// else не нужен — если выше был return, эта строка будет достигнута только когда if не выполнится
		return GetLocalTransform();
	}


} // namespace LEN
