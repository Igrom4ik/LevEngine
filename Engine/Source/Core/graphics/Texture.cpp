
#include "Texture.hpp"

namespace LEN {
    Texture::Texture(int width, int height, int numChannels, const unsigned char *data)
        : m_width(width), m_height(height), m_numChannels(numChannels) {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID); // Activated the texture

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Flags for wrapping S == U
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Flags for wrapping T == V
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // Filtering for minification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering for magnification
    }

    Texture::~Texture() {
        if (m_textureID > 0) {
            glDeleteTextures(1, &m_textureID);
        }
    }

    GLuint Texture::GetTextureID() const {
        return m_textureID;
    }
} // LEN
