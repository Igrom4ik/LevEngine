#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace LEN
{
	class ShaderProgram;

	class Material
	{

	public:
		// Set the shader program used by this material
		void SetShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);
		void SetParam(const std::string& name, float value);
		void Bind();

	private:
		std::shared_ptr<ShaderProgram> m_shaderProgram;
		std::unordered_map<std::string, float> m_floatParams; // Example property: float values
	};

}