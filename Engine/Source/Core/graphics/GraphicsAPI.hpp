#pragma once
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <memory>
#include "Core/graphics/Colors.hpp"

namespace LEN {
	class ShaderProgram;
	class Material;
	class Mesh;

	class GraphicsAPI {
	public:
		bool Init();

		std::shared_ptr<ShaderProgram> CreateShaderProgram(const std::string &vertexSource,
		                                                   const std::string &fragmentSource);

		const std::shared_ptr<ShaderProgram> &GetDefaultShaderProgram() const;

		GLuint CreateVertexBuffer(const std::vector<float> &vertices);

		GLuint CreateIndexBuffer(const std::vector<uint32_t> &indices);

		void SetColor(Color color, float a = 1.0f);

		void ClearBuffers();

		void BindShaderProgram(ShaderProgram *shderProgram);

		void BindMaterial(Material *material);

		void BindMesh(Mesh *mesh);

		void DrawMesh(Mesh *mesh);

	private:
		std::shared_ptr<ShaderProgram> m_defaultShaderProgram;
		bool m_initialized = false;
	};
}
