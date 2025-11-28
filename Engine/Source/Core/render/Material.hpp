#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "fwd.hpp"

namespace LEN {
	class ShaderProgram;
	class Texture;

	class Material {
	public:
		// Set the shader program used by this material
		ShaderProgram *GetShaderProgram();

		void SetShaderProgram(const std::shared_ptr<ShaderProgram> &shaderProgram);

		void SetParam(const std::string &name, float value);

		void SetParam(const std::string &name, float v0, float v1);

		void SetParam(const std::string &name, const std::shared_ptr<Texture> &texture);

		void Bind();

	private:
		std::shared_ptr<ShaderProgram> m_shaderProgram;
		std::unordered_map<std::string, float> m_floatParams; // Example property: float values
		std::unordered_map<std::string, std::pair<float, float> > m_float2Params; // Example property: vec2 values
		std::unordered_map<std::string, std::shared_ptr<Texture> > m_textures; // Example property: texture file paths
	};
}
