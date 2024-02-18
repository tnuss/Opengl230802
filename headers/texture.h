#pragma once

//#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <shader.h>


class Texture
{
public:
    Texture(const std::string& filename, Shader& shader, unsigned int unit);

    // --- need to keep track of which texture unit that is being used??????
    void Bind();

    inline bool IsTextureLoaded() {
        if (m_texture > 0 && m_unit>=0)
            return true;
        else
            return false;
    }
            
    virtual ~Texture();

private:
    Texture(const Texture& other);
    Texture& operator=(const Texture& other);

    GLuint m_unit = -1;
    GLuint m_texture = 0;

};
