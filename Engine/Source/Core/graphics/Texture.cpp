
#include "Texture.hpp"
#include "Core/Engine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace LEN {
    Texture::Texture(int width, int height, int numChannels, const unsigned char *data)
        : m_width(width), m_height(height), m_numChannels(numChannels) {
        Init(width, height, numChannels, data);
    }

    Texture::~Texture() {
        if (m_textureID > 0) {
            glDeleteTextures(1, &m_textureID);
        }
    }

    GLuint Texture::GetTextureID() const {
        return m_textureID;
    }

    void Texture::Init(int width, int height, int numChannels, const unsigned char *data) {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID); // Activated the texture

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Flags for wrapping S == U
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Flags for wrapping T == V
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // Filtering for minification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering for magnification
    }

    std::shared_ptr<Texture> Texture::Load(const std::string &filePath) {
        int width, height, numChannels;

        auto &fs = Engine::GetInstance().GetFileSystem();
        auto fullPath = fs.GetAssetsFolder() / filePath;
        if (!std::filesystem::exists(fullPath)) {
            std::cerr << "Texture::Load(): File not found: " << fullPath << std::endl;
            return nullptr;
        }
        std::shared_ptr<Texture> result;
        unsigned char *data = stbi_load(fullPath.string().c_str(), &width, &height, &numChannels, 0);
        if (data) {
            result = std::make_shared<Texture>(width, height, numChannels, data);
            stbi_image_free(data);
        }
        return result;
    }
} // LEN
