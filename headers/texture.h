#pragma once

//#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>


class Texture
{
public:
    Texture(const std::string& filename);


    // --- need to keep track of which texture unit that is being used??????
    void Bind(unsigned int unit);

    inline bool IsTextureLoaded() {
        if (m_texture > 0)
            return true;
        else
            return false;
    }
            
    virtual ~Texture();

private:
    Texture(const Texture& other);
    Texture& operator=(const Texture& other);

    GLuint m_texture = 0;

};
