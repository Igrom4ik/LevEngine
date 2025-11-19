#include "Core/render/Render.hpp"
#include "Core/render/Mesh.hpp"
#include "Core/render/Material.hpp"
#include "Core/graphics/GraphicsAPI.hpp"



namespace LEN
{
	void RenderQueue::Submit(const RenderCommand& command)
	{
		m_commands.push_back(command);
	}

	void RenderQueue::Draw(GraphicsAPI& graphicsAPI)
	{
		for (auto& command : m_commands)
		{
			graphicsAPI.BindMaterial(command.material);
		}
	}
}