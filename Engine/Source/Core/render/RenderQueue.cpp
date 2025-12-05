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
			// Validate command
			if (!command.material) {
				std::cerr << "RenderQueue::Draw(): skipping command with null material" << std::endl;
				continue;
			}
			if (!command.mesh) {
				std::cerr << "RenderQueue::Draw(): skipping command with null mesh" << std::endl;
				continue;
			}
			if (!command.mesh->IsGPUReady()) {
				std::cerr << "RenderQueue::Draw(): skipping command because mesh GPU resources not ready - VAO=" <<
						command.mesh->GetVAO() << " VBO=" << command.mesh->GetVBO() << " EBO=" << command.mesh->GetEBO()
						<< std::endl;
				continue;
			}

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

			// Bind and draw with exception safety
			try {
				graphicsAPI.BindMesh(command.mesh);
				graphicsAPI.DrawMesh(command.mesh);
			} catch (const std::exception &ex) {
				std::cerr << "RenderQueue::Draw(): exception while drawing mesh: " << ex.what() << std::endl;
				continue;
			} catch (...) {
				std::cerr << "RenderQueue::Draw(): unknown exception while drawing mesh" << std::endl;
				continue;
			}
		}

		m_commands.clear();
	}
}
