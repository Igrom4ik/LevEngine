#include "Core/render/RenderQueue.hpp"
#include "Core/render/Mesh.hpp"
#include "Core/render/Material.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/graphics/ShaderProgram.hpp"



namespace LEN
{
	void RenderQueue::Submit(const RenderCommand& command)
	{
		m_commands.push_back(command);
	}

	void RenderQueue::Draw(GraphicsAPI& graphicsAPI, const CameraData& cameraData)
	{
		for (auto& command : m_commands)
		{
			graphicsAPI.BindMaterial(command.material);
			auto shaderProgram = command.material->GetShaderProgram();
			shaderProgram->SetUniform("uModel", command.modelMatrix);
			shaderProgram->SetUniform("uView", cameraData.viewMatrix);
			shaderProgram->SetUniform("uProjection", cameraData.projectionMatrix);
			graphicsAPI.BindMesh(command.mesh);
			graphicsAPI.DrawMesh(command.mesh);
		}

		m_commands.clear();
	}
}