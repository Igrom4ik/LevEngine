#pragma once
#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <memory>

namespace LEN
{
	class ShaderProgram;
	class Material;

	class GraphicsApi
	{
	public:
		std::shared_ptr<ShaderProgram> CreateShaderProgram(const std::string& vertexSource, 
			const std::string& fragmentSource); 
		GLuint CreateVertexBuffer(const std::vector<float>& vertices);
		GLuint CreateIndexBuffer(const std::vector<uint32_t>& indices);

		void BindShaderProgram(ShaderProgram* shderProgram);
		void BindMaterial(Material* material);


	};
}