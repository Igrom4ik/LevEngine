#include "Core/render/Material.hpp"
#include "Core/graphics/ShaderProgram.hpp"

namespace LEN
{
	void Material::SetShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram)
	{
		m_shaderProgram = shaderProgram;
	}

	void Material::SetParam(const std::string& name, float value)
	{
		m_floatParams[name] = value; // Store the float property
	}

	void Material::Bind()
	{
		if (!m_shaderProgram)
		{
			return;
		}
		m_shaderProgram->Bind();

		for (const auto& praram : m_floatParams)
		{
			m_shaderProgram->SetUniform(praram.first, praram.second); // Set float uniform
		}
	}

}