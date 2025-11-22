#pragma once
#include <vector>
#include <glm/mat4x4.hpp>


namespace LEN
{
	class Mesh;
	class Material;
	class GraphicsAPI;

	struct RenderCommand
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		glm::mat4 modelMatrix;
	};

	class RenderQueue
	{
	public:
		void Submit(const RenderCommand& command); // Submit a render command to the queue
		void Draw(GraphicsAPI& graphicsAPI); // Draw all submitted commands
		
	private:
		std::vector<RenderCommand> m_commands;
	
	};
}