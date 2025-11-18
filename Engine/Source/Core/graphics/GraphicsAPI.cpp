#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/graphics/ShaderProgram.hpp"
#include "Core/render/Material.hpp"
#include <iostream>



namespace LEN
{
	std::shared_ptr<ShaderProgram> LEN::GraphicsApi::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) 
	{
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexShaderCStr = vertexSource.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderCStr, nullptr);
        glCompileShader(vertexShader);

        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			return nullptr;
        }


        std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vColor;
        uniform vec4 uColor;        

        void main()
        {
            FragColor = vec4(vColor, 1.0f) * uColor;
        }
    )";

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentShaderSCStr = fragmentSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSCStr, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            return nullptr;
        }


        GLuint shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertexShader);
        glAttachShader(shaderProgramID, fragmentShader);
        glLinkProgram(shaderProgramID);

        glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
                infoLog << std::endl;
			return nullptr;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
		return std::make_shared<ShaderProgram>(shaderProgramID); // Return the created ShaderProgram
    
    }

    GLuint GraphicsApi::CreateVertexBuffer(const std::vector<float>& vertices)
    {
        GLuint VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
		return VBO;
    }

    GLuint GraphicsApi::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        GLuint EBO = 0;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return EBO;
    }

    void GraphicsApi::BindShaderProgram(ShaderProgram* shderProgram)
    {
        if (shderProgram)
        {
            shderProgram->Bind();
        }
    }

    void GraphicsApi::BindMaterial(Material* material)
    {
        if (material)
        {
			material->Bind();
        }
    }
}
