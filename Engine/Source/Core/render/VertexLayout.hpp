#pragma once
#include <GL/glew.h>
#include <vector>
#include <cstdint>

namespace LEN
{
	struct VertexElement
	{
		GLuint index;        // Attribute location in the shader
		GLuint size;         // Number of components (e.g., 3 for vec3)
		GLuint type;         // Data type (e.g., GL_FLOAT)
		uint32_t offset;      // Bytes offset from the start of the vertex
	};

	struct VertexLayout
	{
		std::vector<VertexElement> elements;
		uint32_t stride = 0; // Total size of a single vertex in bytes

	};
}