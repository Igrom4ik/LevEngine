#include "Core/graphics/ShaderProgram.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Texture.hpp"
#include <iostream>

namespace LEN {
	ShaderProgram::ShaderProgram(GLuint shaderProgramID) : m_shaderProgramID(shaderProgramID) {
	}

	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(m_shaderProgramID);
	}

	void ShaderProgram::Bind() {
		glUseProgram(m_shaderProgramID);
		m_currentTextureUnit = 0;
		// Debug: print active program id
		std::cerr << "ShaderProgram::Bind() -> program id: " << m_shaderProgramID << std::endl;
	}

	GLint ShaderProgram::GetUniformLocation(const std::string &name) {
		auto it = m_uniformLocationCache.find(name);
		if (it != m_uniformLocationCache.end()) {
			return it->second;
		}
		GLint location = glGetUniformLocation(m_shaderProgramID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: uniform '" << name << "' not found in program " << m_shaderProgramID << std::endl;
		}
		m_uniformLocationCache[name] = location;
		return location;
	}

	void ShaderProgram::SetUniform(const std::string &name, float value) {
		auto location = GetUniformLocation(name);
		if (location != -1)
			glUniform1f(location, value);
	}

	void ShaderProgram::SetUniform(const std::string &name, float v0, float v1) {
		auto location = GetUniformLocation(name);
		if (location != -1)
			glUniform2f(location, v0, v1);
	}

	void ShaderProgram::SetUniform(const std::string &name, const glm::mat4 &mat) {
		auto location = GetUniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderProgram::SetUniform(const std::string &name, const glm::vec3 value) {
		auto location = GetUniformLocation(name);
		if (location != -1)
			glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void ShaderProgram::SetTexture(const std::string &name, Texture *texture) {
		auto location = GetUniformLocation(name);

		glActiveTexture(GL_TEXTURE0 + m_currentTextureUnit);
		if (!texture) {
			// If texture is null, bind 0 to keep GL state consistent and set uniform to current unit
			glBindTexture(GL_TEXTURE_2D, 0);
			if (location != -1)
				glUniform1i(location, m_currentTextureUnit);
			++m_currentTextureUnit;
			return;
		}
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		if (location != -1)
			glUniform1i(location, m_currentTextureUnit);
		++m_currentTextureUnit; // Move to the next texture unit
	}
}
