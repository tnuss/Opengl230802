#include "Texture.h"
#include "stb/stb_image.h"
#include <iostream>
#include <assert.h>

Texture::Texture(const std::string& filename)
{
    int width, height, numChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &numChannels, 4);
    if (imageData == NULL)
    {
        std::cerr << "Didn't Load Image for Texture: " << filename << '\n';
        return;
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

void Texture::Bind(unsigned int unit)
{
    assert(unit >= 0 && unit < 16);

    // --- need to keep track of which texture unit that is being used??????
    glActiveTexture(GL_TEXTURE0 + unit);

    glBindTexture(GL_TEXTURE_2D, m_texture);
}