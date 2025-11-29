#pragma once
#include <memory>
#include <GL/glew.h>
#include "../graphics/VertexLayout.hpp"
#include <string>

namespace LEN {
	class Mesh {
	public:
		Mesh(const VertexLayout &, const std::vector<float> &vertices, const std::vector<uint32_t> &indices);

		Mesh(const VertexLayout &, const std::vector<float> &vertices);

		Mesh(const Mesh &) = delete;

		Mesh &operator =(const Mesh &) = delete;

		void Bind();

		void Draw();

		static std::shared_ptr<Mesh> Load(const std::string &path);

	private:
		VertexLayout m_vertexLayout;
		GLuint m_VBO = 0; // Vertex Buffer Object
		GLuint m_EBO = 0; // Element Buffer Object
		GLuint m_VAO = 0; // Vertex Array Object

		size_t m_vertexCount = 0;
		size_t m_indexCount = 0;
	};
}
