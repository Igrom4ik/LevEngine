#include "Core/render/RenderQueue.hpp"
#include "Core/render/Mesh.hpp"
#include "Core/render/Material.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/graphics/ShaderProgram.hpp"


namespace LEN {
	void RenderQueue::Submit(const RenderCommand &command) {
		m_commands.push_back(command);
	}

	void RenderQueue::Draw(GraphicsAPI &graphicsAPI, const CameraData &cameraData,
	                       const std::vector<LightData> &lights) {
		for (auto &command: m_commands) {
			graphicsAPI.BindMaterial(command.material);
			auto shaderProgram = command.material->GetShaderProgram();
			if (!shaderProgram) {
				// No shader bound for this material; skip drawing this command
				continue;
			}
			shaderProgram->SetUniform("uModel", command.modelMatrix);
			shaderProgram->SetUniform("uView", cameraData.viewMatrix);
			shaderProgram->SetUniform("uProjection", cameraData.projectionMatrix);

			if (!lights.empty()) {
				auto &light = lights[0];
				// GLSL struct members must be addressed with the struct name, e.g. "uLight.color"
				shaderProgram->SetUniform("uLight.color", light.color);
				shaderProgram->SetUniform("uLight.position", light.position);
			}

			graphicsAPI.BindMesh(command.mesh);
			graphicsAPI.DrawMesh(command.mesh);
		}

		m_commands.clear();
	}
}
