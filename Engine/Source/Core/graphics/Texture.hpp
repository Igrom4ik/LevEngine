#pragma once
#include <GL/glew.h>

namespace LEN {
    class Texture {
    public:
        Texture(int width, int height, int numChannels, const unsigned char *data);

        ~Texture();

        GLuint GetTextureID() const;

    private:
        int m_width = 0, m_height = 0, m_numChannels = 0;
        GLuint m_textureID = 0;
    };
}
