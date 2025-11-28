#include "Core/render/Material.hpp"
#include "Core/graphics/ShaderProgram.hpp"
#include "Core/graphics/Texture.hpp"

namespace LEN {
	LEN::ShaderProgram *Material::GetShaderProgram() {
		return m_shaderProgram.get();
	}

	void Material::SetShaderProgram(const std::shared_ptr<ShaderProgram> &shaderProgram) {
		m_shaderProgram = shaderProgram;
	}

	void Material::SetParam(const std::string &name, float value) {
		m_floatParams[name] = value; // Store the float property
	}

	void Material::SetParam(const std::string &name, float v0, float v1) {
		m_float2Params[name] = {v0, v1}; // Store the vec2 property
	}

	void Material::SetParam(const std::string &name, const std::shared_ptr<Texture> &texture) {
		m_textures[name] = texture;
	}

	void Material::Bind() {
		if (!m_shaderProgram) {
			return;
		}
		m_shaderProgram->Bind();

		for (const auto &praram: m_floatParams) {
			m_shaderProgram->SetUniform(praram.first, praram.second); // Set float uniform
		}

		for (auto &param: m_float2Params) {
			m_shaderProgram->SetUniform(param.first, param.second.first, param.second.second);
		}

		for (const auto &praram: m_textures) {
			m_shaderProgram->SetTexture(praram.first, praram.second.get()); // Set float uniform
		}
	}
}
