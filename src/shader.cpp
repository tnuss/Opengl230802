#pragma once

#include "shader.h"

#include <iostream>
#include <fstream>

Shader::Shader(const std::string& fileName, const std::string& fileName2)
{
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(LoadShader(fileName + ".vert"), GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(LoadShader(fileName2 + ".frag"), GL_FRAGMENT_SHADER);

    AttachValidate();
}

Shader::Shader(const std::string& fileName)
{
    m_program = glCreateProgram();
    m_shaders[0] = CreateShader(LoadShader(fileName + ".vert"), GL_VERTEX_SHADER);
    m_shaders[1] = CreateShader(LoadShader(fileName + ".frag"), GL_FRAGMENT_SHADER);

    for (unsigned int i = 0; i < NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);

    //glBindAttribLocation(m_program, 0, "Position");
    //glBindAttribLocation(m_program, 1, "FragColor");
    // glBindAttribLocation(m_program, 2, "normal");

    glLinkProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

    //vsUnfrmXSizeLoc = glGetUniformLocation(m_program, vsUnfrmXSize.c_str());
    //if (vsUnfrmXSizeLoc == -1) {
    //    printf("Error getting uniform location of 'vsXSize'\n");
    //    exit(1);
    //}

    //vsUnfrmYSizeLoc = glGetUniformLocation(m_program, vsUnfrmYSize.c_str());
    //if (vsUnfrmYSizeLoc == -1) {
    //    printf("Error getting uniform location of 'vsYSize' \n");
    //    exit(1);
    //}

    //vsUnfmXDimScaleLoc = glGetUniformLocation(m_program, vsUnfmXDimScale.c_str());
    //if (vsUnfmXDimScaleLoc == -1) {
    //    printf("Error getting uniform location of 'vsXDimScale' \n");
    //    exit(1);
    //}

    //gTranslationLocation = glGetUniformLocation(m_program, "gTranslation");
    //if (gTranslationLocation == -1) {
    //    printf("Error getting uniform location of 'gTranslation'\n");
    //    exit(1);
    //}
    // 
    //GLuint vsMVMatrix;
    //GLuint vsProjMatrix;

    glValidateProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");

    //unfrmTexUnitLoc = glGetUniformLocation(m_program, "sh_texunit");
    //if (unfrmTexUnitLoc == -1) {
    //    printf("Error getting uniform location of 'sh_texunit'\n");
    //    exit(1);
    //}

    //vsMVLoc = glGetUniformLocation(m_program, "mv_matrix");
    //if (vsMVLoc == -1) {
    //    printf("Error getting uniform location of 'vsMVLoc'\n");
    //    exit(1);
    //}

    //vsProjLoc = glGetUniformLocation(m_program, "proj_matrix");
    //if (vsProjLoc == -1) {
    //    printf("Error getting uniform location of 'vsProjLoc'\n");
    //    exit(1);
    //}

    //vsVLoc = glGetUniformLocation(m_program, "v_matrix");
    //if (vsProjLoc == -1) {
    //    printf("Error getting uniform location of 'vsVLoc'\n");
    //    exit(1);
    //}

    //glValidateProgram(m_program);
    //CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");

    //m_uniforms[0] = glGetUniformLocation(m_program, "MVP");
    //m_uniforms[1] = glGetUniformLocation(m_program, "Normal");
    //m_uniforms[2] = glGetUniformLocation(m_program, "lightDirection");
}

Shader::~Shader()
{
    for (unsigned int i = 0; i < NUM_SHADERS; i++)
    {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

    glDeleteProgram(m_program);
}

void Shader::AttachValidate()
{
    for (unsigned int i = 0; i < NUM_SHADERS; i++)
        glAttachShader(m_program, m_shaders[i]);

    glLinkProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

    glValidateProgram(m_program);
    CheckShaderError(m_program, GL_LINK_STATUS, true, "Invalid shader program");
}

void Shader::Bind()
{
    glUseProgram(m_program);
}

//void Shader::Update(const Transform& transform, const Camera& camera)
//{
//    glm::mat4 MVP = transform.GetMVP(camera);
//    glm::mat4 Normal = transform.GetModel();
//
//    glUniformMatrix4fv(m_uniforms[0], 1, GL_FALSE, &MVP[0][0]);
//    glUniformMatrix4fv(m_uniforms[1], 1, GL_FALSE, &Normal[0][0]);
//    glUniform3f(m_uniforms[2], 0.0f, 0.0f, 1.0f);
//}

std::string Shader::LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if (file.is_open())
    {
        while (file.good())
        {
            getline(file, line);
            output.append(line + "\n");
        }
    }
    else
    {
        std::cerr << "Unable to load shader: " << fileName << "\n";
    }

    return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if (isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if (success == GL_FALSE)
    {
        if (isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << "\n";
    }
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader = glCreateShader(type);

    if (shader == 0)
        std::cerr << "Error compiling shader type " << type << "\n";

    const GLchar* p[1];
    p[0] = text.c_str();
    GLint lengths[1];
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}
