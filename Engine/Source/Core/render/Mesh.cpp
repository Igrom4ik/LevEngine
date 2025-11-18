#include "Core/render/Mesh.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/Engine.hpp"
#include <GL/glew.h>
#include <cstdint>


namespace LEN
{
	Mesh::Mesh(const VertexLayout& layout, const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		m_vertexLayout = layout;

		auto& graphicsAPI = Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance

		// Create and upload vertex/index buffers using GraphicsAPI
		m_VBO = graphicsAPI.CreateVertexBuffer(vertices);
		m_EBO = graphicsAPI.CreateIndexBuffer(indices);

		glGenVertexArrays(1, &m_VAO); // Generate VAO
		glBindVertexArray(m_VAO); // Bind VAO

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO

		for (auto& element : m_vertexLayout.elements)
		{
			glVertexAttribPointer(
				element.index,
				element.size,
				element.type,
				GL_FALSE,
				static_cast<GLsizei>(m_vertexLayout.stride),
				reinterpret_cast<void*>(static_cast<uintptr_t>(element.offset))
			);
			glEnableVertexAttribArray(element.index);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		// Set 0 for Buffer's
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_vertexCount = (vertices.size() + sizeof(float)) / m_vertexLayout.stride;
		m_indexCount = indices.size();
	}

	Mesh::Mesh(const VertexLayout& layout, const std::vector<float>& vertices)
	{
		m_vertexLayout = layout;

		auto& graphicsAPI = Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance

		// Create and upload vertex/index buffers using GraphicsAPI
		m_VBO = graphicsAPI.CreateVertexBuffer(vertices);

		glGenVertexArrays(1, &m_VAO); // Generate VAO
		glBindVertexArray(m_VAO); // Bind VAO

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO

		for (auto& element : m_vertexLayout.elements)
		{
			glVertexAttribPointer(
				element.index,
				element.size,
				element.type,
				GL_FALSE,
				static_cast<GLsizei>(m_vertexLayout.stride),
				reinterpret_cast<void*>(static_cast<uintptr_t>(element.offset))
			);
			glEnableVertexAttribArray(element.index);
		}
		// Set 0 for Buffer's
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_vertexCount = (vertices.size() + sizeof(float)) / m_vertexLayout.stride;

	}

	void Mesh::Bind()
	{
		glBindVertexArray(m_VAO);

	}

	void Mesh::Draw()
	{
		if (m_indexCount > 0)
		{
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
		}
	}
}

