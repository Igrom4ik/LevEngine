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
		ShaderProgram* GetShaderProgram();
		void SetShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);
		void SetParam(const std::string& name, float value);
		void SetParam(const std::string& name, float v0, float v1);
		void Bind();


	private:
		std::shared_ptr<ShaderProgram> m_shaderProgram;
		std::unordered_map<std::string, float> m_floatParams; // Example property: float values
		std::unordered_map<std::string, std::pair<float, float>> m_flaot2Params; // Example property: vec2 values
	};

}