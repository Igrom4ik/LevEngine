#pragma once
#include <vector>


namespace LEN
{
	class Mesh;
	class Material;
	class GraphicsAPI;

	struct RenderCommand
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
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