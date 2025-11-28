#pragma once
#include <memory>
#include <GL/glew.h>
#include <string>

namespace LEN {
    class Texture {
    public:
        Texture(int width, int height, int numChannels, const unsigned char *data);

        ~Texture();

        GLuint GetTextureID() const;

        void Init(int width, int height, int numChannels, const unsigned char *data);

        static std::shared_ptr<Texture> Load(const std::string &filePath);

    private:
        int m_width = 0, m_height = 0, m_numChannels = 0;
        GLuint m_textureID = 0;
    };
}
