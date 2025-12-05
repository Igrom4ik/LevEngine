#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/graphics/ShaderProgram.hpp"
#include "Core/render/Material.hpp"
#include "Core/render/Mesh.hpp"
#include <iostream>
#include <stdexcept>

#if defined(TRACY_ENABLE)
#include <tracy/Tracy.hpp>
#endif


namespace LEN {
    bool GraphicsAPI::Init() {
        glEnable(GL_DEPTH_TEST);
        m_initialized = true;
        return true;
    }

    std::shared_ptr<ShaderProgram> LEN::GraphicsAPI::CreateShaderProgram(
        const std::string &vertexSource, const std::string &fragmentSource) {
        if (!m_initialized) {
            std::cerr << "GraphicsAPI::CreateShaderProgram(): OpenGL not initialized!" << std::endl;
            throw std::runtime_error("CreateShaderProgram called before GraphicsAPI::Init");
        }
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *vertexShaderCStr = vertexSource.c_str();
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


        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fragmentShaderSCStr = fragmentSource.c_str();
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

    const std::shared_ptr<ShaderProgram> &GraphicsAPI::GetDefaultShaderProgram() const {
        static std::shared_ptr<ShaderProgram> defaultProgram;
        if (!defaultProgram) {
            std::string vertexShaderSource = R"(
                #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;
        layout (location = 2) in vec2 uv;
        layout (location = 3) in vec3 normal;

        out vec2 vUV;
        out vec3 vNormal;
        out vec3 vFragPos;


        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;

        void main()
        {
            vUV = uv;
            vFragPos = vec3(uModel * vec4(position, 1.0));

            vNormal = mat3(transpose(inverse(uModel))) * normal;// Transform normal to world space

            gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
        }
    )";

            std::string fragmentShaderSource = R"(
        #version 330 core
        struct Light {
            vec3 color;
            vec3 position;
        };
        uniform Light uLight;

        out vec4 FragColor;

        in vec2 vUV;
        in vec3 vNormal;
        in vec3 vFragPos;

        uniform sampler2D baseColorTexture;

        void main()
        {
            vec3 norm = normalize(vNormal);
            vec3 lightDir = normalize(uLight.position - vFragPos);

            float diff = (max(dot(norm, lightDir), 0.0));

            vec3 diffuse = diff * uLight.color;

            vec4 texColor = texture(baseColorTexture, vUV);

            FragColor = texColor * vec4(diffuse, 1.0);
        }
    )";
            // вызываем неконстантный метод через const_cast, чтобы не менять сигнатуру метода
            defaultProgram = const_cast<GraphicsAPI *>(this)->CreateShaderProgram(
                vertexShaderSource, fragmentShaderSource);
        }

        return defaultProgram;
    }

    GLuint GraphicsAPI::CreateVertexBuffer(const std::vector<float> &vertices) {
        if (!m_initialized) {
            std::cerr << "GraphicsAPI::CreateVertexBuffer(): OpenGL not initialized!" << std::endl;
            throw std::runtime_error("CreateVertexBuffer called before GraphicsAPI::Init");
        }
        GLuint VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return VBO;
    }

    GLuint GraphicsAPI::CreateIndexBuffer(const std::vector<uint32_t> &indices) {
        if (!m_initialized) {
            std::cerr << "GraphicsAPI::CreateIndexBuffer(): OpenGL not initialized!" << std::endl;
            throw std::runtime_error("CreateIndexBuffer called before GraphicsAPI::Init");
        }
        GLuint EBO = 0;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return EBO;
    }

    void GraphicsAPI::BindShaderProgram(ShaderProgram *shderProgram) {
        if (shderProgram) {
            shderProgram->Bind();
        }
    }

    void GraphicsAPI::BindMaterial(Material *material) {
        if (material) {
            material->Bind();
        }
    }

    void GraphicsAPI::BindMesh(Mesh *mesh) {
        if (mesh) {
            mesh->Bind();
        }
    }

    void GraphicsAPI::DrawMesh(Mesh *mesh) {
#if defined(TRACY_ENABLE)
        ZoneScoped;
#endif
        if (mesh) {
            mesh->Draw();
        }
    }

    void GraphicsAPI::SetColor(Color color, float a) {
        const ColorRGB c = LEN::GetColorRGB(color);
        glClearColor(c.r, c.g, c.b, a);
    }

    void GraphicsAPI::ClearBuffers() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
